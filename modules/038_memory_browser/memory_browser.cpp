#include "memory_browser.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../023_personal_memory/personal_memory.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <set>
#include <sstream>
#include <unordered_set>

namespace memory_browser {
namespace {

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

double parse_confidence(const std::string & source) {
    auto pos = source.find("confidence=");
    if (pos == std::string::npos) return 1.0;
    pos += std::string("confidence=").size();
    auto end = source.find(' ', pos);
    std::string num = source.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
    try { return std::stod(num); } catch (...) { return 1.0; }
}

std::string year_month(int64_t ts) {
    std::time_t t = (std::time_t)ts;
    std::tm tm{}; ::gmtime_r(&t, &tm);
    char buf[16]; std::strftime(buf, sizeof(buf), "%Y-%m", &tm);
    return buf;
}

EntryRow make_row(Scope sc, const project_memory::Entry & e) {
    EntryRow r;
    r.scope    = sc;
    r.id       = e.id; r.ts = e.ts; r.category = e.category;
    r.content  = e.content; r.tags = e.tags; r.source = e.source;
    r.confidence = parse_confidence(e.source);
    return r;
}
EntryRow make_row(Scope sc, const personal_memory::Entry & e) {
    EntryRow r;
    r.scope    = sc;
    r.id       = e.id; r.ts = e.ts; r.category = e.category;
    r.content  = e.content; r.tags = e.tags; r.source = e.source;
    r.confidence = parse_confidence(e.source);
    return r;
}

bool matches(const EntryRow & r, const Filter & f) {
    if (f.scope != Scope::Both && r.scope != f.scope) return false;
    if (!f.categories.empty()) {
        bool ok = false;
        for (const auto & c : f.categories) if (c == r.category) { ok = true; break; }
        if (!ok) return false;
    }
    if (!f.tags_all.empty()) {
        std::unordered_set<std::string> have(r.tags.begin(), r.tags.end());
        for (const auto & t : f.tags_all) if (!have.count(t)) return false;
    }
    if (!f.tags_any.empty()) {
        std::unordered_set<std::string> have(r.tags.begin(), r.tags.end());
        bool ok = false;
        for (const auto & t : f.tags_any) if (have.count(t)) { ok = true; break; }
        if (!ok) return false;
    }
    if (!f.text_substring.empty()) {
        if (lower(r.content).find(lower(f.text_substring)) == std::string::npos) return false;
    }
    if (f.ts_min && r.ts < f.ts_min) return false;
    if (f.ts_max && r.ts > f.ts_max) return false;
    if (r.confidence < f.confidence_min - 1e-12) return false;
    return true;
}

std::vector<EntryRow> all_rows(const Filter & f, std::string_view project_root) {
    std::vector<EntryRow> out;
    if (f.scope == Scope::Project || f.scope == Scope::Both) {
        for (auto & e : project_memory::list({}, {}, 100000))
            out.push_back(make_row(Scope::Project, e));
    }
    if (f.scope == Scope::Personal || f.scope == Scope::Both) {
        for (auto & e : personal_memory::list({}, {}, 100000))
            out.push_back(make_row(Scope::Personal, e));
    }
    (void)project_root;
    return out;
}

}

const char * to_string(Facet f) {
    switch (f) {
        case Facet::Scope:     return "scope";
        case Facet::Category:  return "category";
        case Facet::Tag:       return "tag";
        case Facet::Source:    return "source";
        case Facet::YearMonth: return "year-month";
    }
    return "category";
}

void init()     {}
void shutdown() {}

std::vector<EntryRow> query(const Filter & f, std::string_view project_root,
                            size_t limit) {
    auto rows = all_rows(f, project_root);
    std::vector<EntryRow> matched;
    matched.reserve(rows.size());
    for (auto & r : rows) if (matches(r, f)) matched.push_back(std::move(r));
    std::sort(matched.begin(), matched.end(),
              [](const EntryRow & a, const EntryRow & b){ return a.ts > b.ts; });
    if (limit && matched.size() > limit) matched.resize(limit);
    return matched;
}

std::map<std::string, size_t> facet_counts(const Filter & f, Facet facet,
                                           std::string_view project_root) {
    std::map<std::string, size_t> counts;
    auto rows = all_rows(f, project_root);
    for (const auto & r : rows) {
        if (!matches(r, f)) continue;
        switch (facet) {
            case Facet::Scope:
                ++counts[r.scope == Scope::Project ? "project" : "personal"];
                break;
            case Facet::Category:
                ++counts[r.category];
                break;
            case Facet::Tag:
                for (const auto & t : r.tags) ++counts[t];
                break;
            case Facet::Source: {
                // Bucket by source prefix up to ':' so "chat:turn-3" and
                // "chat:turn-7" share a bucket.
                std::string s = r.source;
                auto colon = s.find(':');
                if (colon != std::string::npos) s = s.substr(0, colon);
                if (s.empty()) s = "(none)";
                ++counts[s];
                break;
            }
            case Facet::YearMonth:
                ++counts[year_month(r.ts)];
                break;
        }
    }
    return counts;
}

}
