#include "ref_formats.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_map>

namespace ref_formats {
namespace {

std::string trim(std::string_view in) {
    size_t a = 0, b = in.size();
    while (a < b && std::isspace(static_cast<unsigned char>(in[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(in[b - 1]))) --b;
    return std::string(in.substr(a, b - a));
}

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

// Map BibTeX entry type -> CSL type.
std::string bib_to_csl_type(const std::string & btype) {
    static const std::unordered_map<std::string, std::string> m = {
        {"article",       "article-journal"},
        {"book",          "book"},
        {"inbook",        "chapter"},
        {"incollection",  "chapter"},
        {"inproceedings", "paper-conference"},
        {"conference",    "paper-conference"},
        {"manual",        "report"},
        {"techreport",    "report"},
        {"thesis",        "thesis"},
        {"phdthesis",     "thesis"},
        {"mastersthesis", "thesis"},
        {"misc",          "document"},
        {"unpublished",   "manuscript"},
        {"online",        "webpage"},
        {"electronic",    "webpage"},
        {"patent",        "patent"},
        {"software",      "software"},
    };
    auto it = m.find(lower(btype));
    return it == m.end() ? "document" : it->second;
}

std::string csl_to_bib_type(const std::string & ct) {
    static const std::unordered_map<std::string, std::string> m = {
        {"article-journal", "article"},
        {"article",         "article"},
        {"book",            "book"},
        {"chapter",         "inbook"},
        {"paper-conference","inproceedings"},
        {"report",          "techreport"},
        {"thesis",          "phdthesis"},
        {"manuscript",      "unpublished"},
        {"webpage",         "online"},
        {"patent",          "patent"},
        {"software",        "software"},
        {"document",        "misc"},
    };
    auto it = m.find(ct);
    return it == m.end() ? "misc" : it->second;
}

// Strip BibTeX field braces / quotes, collapse spaces, drop one outer
// pair only.
std::string clean_field_value(std::string v) {
    v = trim(v);
    if (!v.empty() && (v.front() == '{' || v.front() == '"')) {
        char open = v.front();
        char close = (open == '{') ? '}' : '"';
        if (v.back() == close && v.size() >= 2) {
            v = v.substr(1, v.size() - 2);
        }
    }
    return trim(v);
}

void split_authors_bib(const std::string & raw, nlohmann::json & out_arr) {
    // BibTeX: separator is " and ".
    std::string s = raw;
    out_arr = nlohmann::json::array();
    size_t pos = 0;
    while (pos <= s.size()) {
        auto sep = s.find(" and ", pos);
        std::string one = (sep == std::string::npos) ? s.substr(pos)
                                                     : s.substr(pos, sep - pos);
        one = trim(one);
        if (!one.empty()) {
            nlohmann::json a;
            auto comma = one.find(',');
            if (comma != std::string::npos) {
                a["family"] = trim(one.substr(0, comma));
                a["given"]  = trim(one.substr(comma + 1));
            } else {
                auto sp = one.rfind(' ');
                if (sp == std::string::npos) a["family"] = one;
                else { a["given"] = trim(one.substr(0, sp));
                       a["family"] = trim(one.substr(sp + 1)); }
            }
            out_arr.push_back(a);
        }
        if (sep == std::string::npos) break;
        pos = sep + 5;     // past " and "
    }
}

void set_issued_year(nlohmann::json & doc, const std::string & year) {
    if (year.empty()) return;
    try {
        int y = std::stoi(year);
        doc["issued"] = { {"date-parts", nlohmann::json::array({{y}})} };
    } catch (...) {
        doc["issued"] = { {"literal", year} };
    }
}

// Parse the value portion of `field = ...,` from `body` starting at
// `pos` (right after the '='). Returns the value bytes (including
// the outer quotes / braces) and advances pos past them.
std::string scan_field_value(const std::string & body, size_t & pos) {
    while (pos < body.size() && std::isspace(static_cast<unsigned char>(body[pos]))) ++pos;
    if (pos >= body.size()) return {};
    char first = body[pos];
    if (first == '{') {
        int depth = 0;
        size_t start = pos;
        for (; pos < body.size(); ++pos) {
            if (body[pos] == '{') ++depth;
            else if (body[pos] == '}') {
                --depth;
                if (depth == 0) { ++pos; return body.substr(start, pos - start); }
            }
        }
        return body.substr(start);
    } else if (first == '"') {
        size_t start = pos++;
        for (; pos < body.size(); ++pos) {
            if (body[pos] == '"' && body[pos - 1] != '\\') {
                ++pos; return body.substr(start, pos - start);
            }
        }
        return body.substr(start);
    } else {
        size_t start = pos;
        for (; pos < body.size(); ++pos) {
            if (body[pos] == ',' || body[pos] == '\n' || body[pos] == '}') break;
        }
        return body.substr(start, pos - start);
    }
}

// Parse a single @entry{key, field = value, ...} block. `body[pos]`
// must point at the '@'. On return, pos is just past the closing '}'.
nlohmann::json parse_bib_entry(const std::string & body, size_t & pos) {
    nlohmann::json doc;
    if (pos >= body.size() || body[pos] != '@') return doc;
    ++pos;
    std::string type;
    while (pos < body.size() && std::isalnum(static_cast<unsigned char>(body[pos]))) {
        type.push_back(body[pos]); ++pos;
    }
    while (pos < body.size() && std::isspace(static_cast<unsigned char>(body[pos]))) ++pos;
    if (pos >= body.size() || body[pos] != '{') return doc;
    ++pos;
    // Citation key, up to the first comma.
    std::string key;
    while (pos < body.size() && body[pos] != ',' && body[pos] != '}') {
        key.push_back(body[pos]); ++pos;
    }
    key = trim(key);
    if (!key.empty()) doc["id"] = key;
    if (pos < body.size() && body[pos] == ',') ++pos;
    doc["type"] = bib_to_csl_type(type);

    // Field loop.
    while (true) {
        while (pos < body.size() && (std::isspace(static_cast<unsigned char>(body[pos])) ||
                                      body[pos] == ',')) ++pos;
        if (pos >= body.size() || body[pos] == '}') { ++pos; break; }
        std::string fname;
        while (pos < body.size() && (std::isalnum(static_cast<unsigned char>(body[pos])) ||
                                      body[pos] == '_' || body[pos] == '-')) {
            fname.push_back((char)std::tolower((unsigned char)body[pos])); ++pos;
        }
        while (pos < body.size() && std::isspace(static_cast<unsigned char>(body[pos]))) ++pos;
        if (pos >= body.size() || body[pos] != '=') break;
        ++pos;
        std::string raw = scan_field_value(body, pos);
        std::string val = clean_field_value(raw);

        if (fname == "title")        doc["title"] = val;
        else if (fname == "author")  { nlohmann::json arr; split_authors_bib(val, arr); doc["author"] = arr; }
        else if (fname == "editor")  { nlohmann::json arr; split_authors_bib(val, arr); doc["editor"] = arr; }
        else if (fname == "year")    set_issued_year(doc, val);
        else if (fname == "journal" || fname == "booktitle" || fname == "container")
                                     doc["container-title"] = val;
        else if (fname == "publisher") doc["publisher"] = val;
        else if (fname == "volume")    doc["volume"]    = val;
        else if (fname == "number" || fname == "issue") doc["issue"] = val;
        else if (fname == "pages")     doc["page"]      = val;
        else if (fname == "abstract")  doc["abstract"]  = val;
        else if (fname == "doi")       doc["DOI"]       = val;
        else if (fname == "url")       doc["URL"]       = val;
        else if (fname == "isbn")      doc["ISBN"]      = val;
        else if (fname == "issn")      doc["ISSN"]      = val;
        else if (fname == "keywords")  doc["keyword"]   = val;
        else                            doc["x-bibtex-" + fname] = val;
    }
    return doc;
}

}

void init()     {}
void shutdown() {}

std::vector<nlohmann::json> parse_bibtex(std::string_view body_v) {
    std::string body{body_v};
    std::vector<nlohmann::json> out;
    size_t pos = 0;
    while (pos < body.size()) {
        if (body[pos] == '@') {
            auto doc = parse_bib_entry(body, pos);
            if (!doc.empty()) out.push_back(doc);
        } else { ++pos; }
    }
    return out;
}

std::string to_bibtex(const nlohmann::json & item) {
    std::string id = item.value("id", "noid");
    std::string ct = item.value("type", "document");
    std::string bt = csl_to_bib_type(ct);
    std::ostringstream os;
    os << "@" << bt << "{" << id << ",\n";

    auto put = [&](const char * fname, const std::string & val) {
        if (val.empty()) return;
        os << "  " << fname << " = {" << val << "},\n";
    };
    if (item.contains("title"))           put("title",     item["title"].get<std::string>());
    if (item.contains("container-title")) put("journal",   item["container-title"].get<std::string>());
    if (item.contains("publisher"))       put("publisher", item["publisher"].get<std::string>());
    if (item.contains("volume"))          put("volume",    item["volume"].get<std::string>());
    if (item.contains("issue"))           put("number",    item["issue"].get<std::string>());
    if (item.contains("page"))            put("pages",     item["page"].get<std::string>());
    if (item.contains("abstract"))        put("abstract",  item["abstract"].get<std::string>());
    if (item.contains("DOI"))             put("doi",       item["DOI"].get<std::string>());
    if (item.contains("URL"))             put("url",       item["URL"].get<std::string>());
    if (item.contains("ISBN"))            put("isbn",      item["ISBN"].get<std::string>());

    if (item.contains("author") && item["author"].is_array()) {
        std::string s;
        bool first = true;
        for (const auto & a : item["author"]) {
            if (!a.is_object()) continue;
            std::string fam = a.value("family", "");
            std::string giv = a.value("given",  "");
            if (fam.empty() && giv.empty()) continue;
            if (!first) s += " and ";
            first = false;
            if (giv.empty()) s += fam;
            else             s += fam + ", " + giv;
        }
        put("author", s);
    }
    if (item.contains("issued")) {
        const auto & iss = item["issued"];
        std::string y;
        if (iss.contains("date-parts") && iss["date-parts"].is_array() &&
            !iss["date-parts"].empty() && iss["date-parts"][0].is_array() &&
            !iss["date-parts"][0].empty()) {
            const auto & v = iss["date-parts"][0][0];
            if (v.is_number_integer()) y = std::to_string(v.get<int>());
            if (v.is_string())         y = v.get<std::string>();
        }
        if (!y.empty()) put("year", y);
    }
    os << "}\n";
    return os.str();
}

std::string to_bibtex_many(const std::vector<nlohmann::json> & items) {
    std::string out;
    for (const auto & it : items) out += to_bibtex(it);
    return out;
}

std::vector<nlohmann::json> parse_ris(std::string_view body_v) {
    std::vector<nlohmann::json> out;
    std::string body{body_v};
    std::istringstream is(body);
    std::string line;
    nlohmann::json cur;
    std::vector<std::string> authors;
    bool open = false;

    auto flush = [&]() {
        if (!open) return;
        if (!authors.empty()) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto & a : authors) {
                nlohmann::json one;
                auto comma = a.find(',');
                if (comma != std::string::npos) {
                    one["family"] = trim(a.substr(0, comma));
                    one["given"]  = trim(a.substr(comma + 1));
                } else { one["family"] = trim(a); }
                arr.push_back(one);
            }
            cur["author"] = arr;
        }
        out.push_back(cur);
        cur = nlohmann::json{};
        authors.clear();
        open = false;
    };

    while (std::getline(is, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // RIS tag lines are "XX  - VALUE". The trailing space + value
        // are optional (ER often appears as just "ER  -").
        if (line.size() < 5 || line[2] != ' ' || line[3] != ' ' || line[4] != '-') continue;
        std::string tag = line.substr(0, 2);
        std::string val = line.size() > 6 ? trim(line.substr(6)) : std::string{};
        if (tag == "TY") {
            cur = nlohmann::json::object();
            authors.clear();
            // RIS type -> CSL type, abbreviated.
            std::string c = "document";
            if (val == "JOUR" || val == "EJOUR") c = "article-journal";
            else if (val == "BOOK" || val == "EDBK") c = "book";
            else if (val == "CHAP")     c = "chapter";
            else if (val == "CONF")     c = "paper-conference";
            else if (val == "RPRT")     c = "report";
            else if (val == "THES")     c = "thesis";
            else if (val == "ELEC" || val == "WEB") c = "webpage";
            else if (val == "PAT")      c = "patent";
            cur["type"] = c;
            open = true;
        } else if (tag == "ER") {
            flush();
        } else if (tag == "TI" || tag == "T1") cur["title"] = val;
        else if (tag == "AU" || tag == "A1")    authors.push_back(val);
        else if (tag == "JO" || tag == "T2" || tag == "JF") cur["container-title"] = val;
        else if (tag == "PB")    cur["publisher"]  = val;
        else if (tag == "VL")    cur["volume"]     = val;
        else if (tag == "IS")    cur["issue"]      = val;
        else if (tag == "SP" || tag == "EP") {
            // join start / end pages.
            std::string existing = cur.value("page", "");
            if (existing.empty()) cur["page"] = val;
            else                  cur["page"] = existing + "-" + val;
        }
        else if (tag == "AB" || tag == "N2") cur["abstract"] = val;
        else if (tag == "DO")                cur["DOI"]      = val;
        else if (tag == "UR")                cur["URL"]      = val;
        else if (tag == "SN")                cur["ISSN"]     = val;
        else if (tag == "PY" || tag == "Y1") {
            std::string y = val;
            auto slash = y.find('/');
            if (slash != std::string::npos) y = y.substr(0, slash);
            set_issued_year(cur, y);
        }
    }
    flush();
    return out;
}

std::vector<nlohmann::json> parse_csl_json(std::string_view body) {
    std::vector<nlohmann::json> out;
    try {
        auto j = nlohmann::json::parse(body);
        if (j.is_array()) {
            for (const auto & it : j) if (it.is_object()) out.push_back(it);
        } else if (j.is_object()) {
            out.push_back(j);
        }
    } catch (...) {}
    return out;
}

std::string sniff_format(std::string_view body_v) {
    std::string b{body_v};
    size_t i = 0;
    while (i < b.size() && std::isspace(static_cast<unsigned char>(b[i]))) ++i;
    if (i < b.size() && (b[i] == '[' || b[i] == '{')) return "csl-json";
    if (i < b.size() && b[i] == '@') return "bibtex";
    if (b.find("TY  -") != std::string::npos) return "ris";
    return "unknown";
}

}
