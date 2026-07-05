// SPDX-License-Identifier: GPL-3.0-or-later
#include "bom.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

namespace bom {

using kicad_model::Schematic;
using kicad_model::SchSymbol;
using kicad_model::ItemType;

namespace {

const char * kDefaultColumns[] = {
    "Reference", "Value", "Footprint", "Manufacturer", "MPN", "Datasheet"
};

std::string field_or_empty(const SchSymbol & s, std::string_view name) {
    for (const auto & f : s.fields) if (f.name == name) return f.value;
    return {};
}

bool is_power_symbol(const SchSymbol & s) {
    std::string ref = s.reference();
    if (ref.rfind("#PWR", 0) == 0) return true;
    if (ref.rfind("#FLG", 0) == 0) return true;
    return false;
}

// Sort refs like "R1, R2, R10" naturally (prefix + numeric tail).
struct NatKey {
    std::string prefix;
    int         num;
    std::string raw;
    bool operator<(const NatKey & o) const {
        if (prefix != o.prefix) return prefix < o.prefix;
        return num < o.num;
    }
};
NatKey nat_key(std::string_view ref) {
    NatKey k; k.num = 0; k.raw = std::string(ref);
    std::size_t i = 0;
    while (i < ref.size() && std::isalpha(static_cast<unsigned char>(ref[i]))) k.prefix += ref[i++];
    while (i < ref.size() && std::isdigit(static_cast<unsigned char>(ref[i]))) {
        k.num = k.num * 10 + (ref[i] - '0'); ++i;
    }
    return k;
}

std::string combine_list(std::vector<std::string> refs) {
    std::vector<NatKey> keys;
    for (auto & r : refs) keys.push_back(nat_key(r));
    std::sort(keys.begin(), keys.end());
    std::ostringstream os;
    for (std::size_t i = 0; i < keys.size(); ++i) {
        if (i) os << ", ";
        os << keys[i].raw;
    }
    return os.str();
}

std::string combine_range(std::vector<std::string> refs) {
    std::vector<NatKey> keys;
    for (auto & r : refs) keys.push_back(nat_key(r));
    std::sort(keys.begin(), keys.end());
    std::ostringstream os;
    for (std::size_t i = 0; i < keys.size();) {
        std::size_t j = i;
        while (j + 1 < keys.size() &&
               keys[j+1].prefix == keys[i].prefix &&
               keys[j+1].num    == keys[j].num + 1) ++j;
        if (i) os << ", ";
        if (j > i) os << keys[i].raw << "-" << keys[j].raw;
        else       os << keys[i].raw;
        i = j + 1;
    }
    return os.str();
}

std::string csv_escape(std::string_view s) {
    bool needs = false;
    for (char c : s) if (c == ',' || c == '"' || c == '\n') { needs = true; break; }
    if (!needs) return std::string(s);
    std::string out = "\"";
    for (char c : s) { if (c == '"') out += '"'; out += c; }
    out += '"';
    return out;
}
std::string html_escape(std::string_view s) {
    std::string out; out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '&':  out += "&amp;"; break;
            case '<':  out += "&lt;";  break;
            case '>':  out += "&gt;";  break;
            case '"':  out += "&quot;";break;
            default:   out += c;       break;
        }
    }
    return out;
}
std::string json_escape(std::string_view s) {
    std::string out; out.reserve(s.size() + 2);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

} // namespace

BOM generate(const Schematic & sch, const Options & opts) {
    BOM out;
    if (opts.columns.empty())
        for (const char * c : kDefaultColumns) out.columns.push_back(c);
    else
        out.columns = opts.columns;

    // Group key = (value, footprint, manufacturer, mpn, ...) minus Reference.
    // Reference is always emitted specially.
    auto key_for = [&](const SchSymbol & s) {
        std::string k;
        for (const auto & col : out.columns) {
            if (col == "Reference") continue;
            k += "|"; k += col; k += "="; k += field_or_empty(s, col);
        }
        return k;
    };

    std::map<std::string, Row> groups;
    for (const auto & it : sch.root.items) {
        if (it->type != ItemType::SchSymbol) continue;
        const auto * s = static_cast<const SchSymbol *>(it.get());
        if (opts.exclude_dnp && s->dnp) continue;
        if (opts.exclude_power_symbols && is_power_symbol(*s)) continue;
        auto & row = groups[key_for(*s)];
        row.refs.push_back(s->reference());
        ++row.count;
        if (row.cells.empty()) {
            for (const auto & col : out.columns) {
                if (col == "Reference") continue;
                row.cells.push_back({col, field_or_empty(*s, col)});
            }
        }
    }
    for (auto & kv : groups) {
        auto & row = kv.second;
        std::string combined = (opts.ref_combine == "range")
            ? combine_range(row.refs)
            : combine_list (row.refs);
        // Prepend the Reference cell.
        row.cells.insert(row.cells.begin(), { std::string("Reference"), combined });
        out.rows.push_back(std::move(row));
    }
    // Sort rows by the first Reference for stability.
    std::sort(out.rows.begin(), out.rows.end(), [](const Row & a, const Row & b) {
        return a.cells.front().second < b.cells.front().second;
    });
    return out;
}

std::string to_csv(const BOM & b) {
    std::ostringstream os;
    os << "Qty";
    for (const auto & c : b.columns) os << "," << csv_escape(c);
    os << "\n";
    for (const auto & row : b.rows) {
        os << row.count;
        for (const auto & col : b.columns) {
            std::string v;
            for (const auto & cell : row.cells) if (cell.first == col) { v = cell.second; break; }
            os << "," << csv_escape(v);
        }
        os << "\n";
    }
    return os.str();
}

std::string to_html(const BOM & b) {
    std::ostringstream os;
    os << "<table class=\"eda-bom\">\n<thead><tr><th>Qty</th>";
    for (const auto & c : b.columns) os << "<th>" << html_escape(c) << "</th>";
    os << "</tr></thead>\n<tbody>\n";
    for (const auto & row : b.rows) {
        os << "<tr><td>" << row.count << "</td>";
        for (const auto & col : b.columns) {
            std::string v;
            for (const auto & cell : row.cells) if (cell.first == col) { v = cell.second; break; }
            os << "<td>" << html_escape(v) << "</td>";
        }
        os << "</tr>\n";
    }
    os << "</tbody></table>\n";
    return os.str();
}

std::string to_json(const BOM & b) {
    std::ostringstream os;
    os << "{\n  \"columns\": [";
    for (std::size_t i = 0; i < b.columns.size(); ++i) {
        if (i) os << ", ";
        os << "\"" << json_escape(b.columns[i]) << "\"";
    }
    os << "],\n  \"rows\": [\n";
    for (std::size_t i = 0; i < b.rows.size(); ++i) {
        const auto & row = b.rows[i];
        os << "    { \"count\": " << row.count << ", \"cells\": {";
        bool first = true;
        for (const auto & col : b.columns) {
            std::string v;
            for (const auto & cell : row.cells) if (cell.first == col) { v = cell.second; break; }
            if (!first) os << ", ";
            first = false;
            os << "\"" << json_escape(col) << "\":\"" << json_escape(v) << "\"";
        }
        os << "} }" << (i + 1 < b.rows.size() ? "," : "") << "\n";
    }
    os << "  ]\n}\n";
    return os.str();
}

} // namespace bom
