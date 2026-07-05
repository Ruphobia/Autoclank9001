// SPDX-License-Identifier: GPL-3.0-or-later
#include "annotator.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>

namespace annotator {

using kicad_model::Schematic;
using kicad_model::SchSymbol;
using kicad_model::ItemType;

std::string prefix_of(std::string_view ref) {
    std::string p;
    for (char c : ref) {
        if (std::isalpha(static_cast<unsigned char>(c))) p += c;
        else break;
    }
    return p;
}

int number_of(std::string_view ref) {
    std::size_t i = 0;
    while (i < ref.size() && std::isalpha(static_cast<unsigned char>(ref[i]))) ++i;
    if (i >= ref.size()) return 0;
    if (!std::isdigit(static_cast<unsigned char>(ref[i]))) return 0;
    int n = 0;
    while (i < ref.size() && std::isdigit(static_cast<unsigned char>(ref[i]))) {
        n = n * 10 + (ref[i] - '0');
        ++i;
    }
    return n;
}

namespace {

bool needs_annotation(const std::string & ref, bool loose) {
    if (ref.empty()) return true;
    if (ref.find('?') != std::string::npos) return true;
    if (!loose) return false;
    // "R" without a number, or "R0" (unassigned convention).
    int n = number_of(ref);
    if (n == 0) return true;
    return false;
}

} // namespace

Result annotate(Schematic & sch, const Options & opts) {
    Result out;

    // Pass 1: enumerate every SchSymbol; track used numbers per prefix.
    struct Cell { SchSymbol * sym; std::string prefix; int number; bool needs; };
    std::vector<Cell> cells;
    std::unordered_map<std::string, std::vector<int>> used_by_prefix;

    for (auto & it : sch.root.items) {
        if (it->type != ItemType::SchSymbol) continue;
        auto * s = static_cast<SchSymbol *>(it.get());
        std::string ref = s->reference();
        std::string pfx = prefix_of(ref);
        int n = number_of(ref);
        bool needs = needs_annotation(ref, opts.loose_unannotated);
        cells.push_back({ s, pfx, n, needs });
        if (!needs && n > 0) used_by_prefix[pfx].push_back(n);
    }

    // Sort cells top-to-bottom then left-to-right for stable output.
    std::sort(cells.begin(), cells.end(), [](const Cell & a, const Cell & b) {
        if (a.sym->at.y != b.sym->at.y) return a.sym->at.y < b.sym->at.y;
        return a.sym->at.x < b.sym->at.x;
    });

    // Pass 2: assign numbers to the cells that need them.
    for (auto & c : cells) {
        if (!c.needs) continue;
        auto & used = used_by_prefix[c.prefix];
        std::sort(used.begin(), used.end());

        int n = std::max(opts.start_at, 1);
        for (int candidate : used) {
            if (candidate == n) ++n;
            else if (candidate > n) break;
        }

        std::string old_ref = c.sym->reference();
        std::string new_ref = c.prefix + std::to_string(n);
        // Write into the Reference field.
        bool set = false;
        for (auto & f : c.sym->fields) {
            if (f.name == "Reference") { f.value = new_ref; set = true; break; }
        }
        if (!set) {
            kicad_model::Field f;
            f.name  = "Reference";
            f.value = new_ref;
            f.at    = c.sym->at;
            f.uuid  = kicad_model::make_uuid();
            c.sym->fields.push_back(f);
        }
        used.push_back(n);
        out.changes.push_back({ c.sym->uuid, old_ref, new_ref });
    }

    return out;
}

} // namespace annotator
