#include "recall_probe.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../023_personal_memory/personal_memory.hpp"
#include "../026_merge_dedup/merge_dedup.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace recall_probe {
namespace {

std::vector<std::string> tokens_in(std::string_view q) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : q) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            cur.push_back((char)std::tolower((unsigned char)c));
        } else {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

std::string build_gloss(std::string_view query, std::string_view content,
                        double bm25, double trig) {
    std::ostringstream os;
    bool first = true;
    auto add = [&](const std::string & s) {
        if (!first) os << "; ";
        os << s; first = false;
    };

    // Identify literal tokens that appear in the content.
    std::string c_lower;
    c_lower.reserve(content.size());
    for (char c : content) c_lower.push_back((char)std::tolower((unsigned char)c));

    std::vector<std::string> seen;
    for (const auto & t : tokens_in(query)) {
        if (t.size() < 3) continue;
        if (c_lower.find(t) != std::string::npos) seen.push_back(t);
    }
    if (!seen.empty()) {
        std::ostringstream ks;
        ks << "matched terms ";
        for (size_t i = 0; i < seen.size(); ++i) { if (i) ks << ", "; ks << "'" << seen[i] << "'"; }
        add(ks.str());
    }
    char buf[64];
    std::snprintf(buf, sizeof(buf), "bm25=%.3f", bm25);
    add(buf);
    std::snprintf(buf, sizeof(buf), "trigram=%.2f", trig);
    add(buf);
    return os.str();
}

}

void init()     {}
void shutdown() {}

std::vector<Hit> probe(Scope scope, std::string_view query,
                       size_t top_k, size_t fts_pool) {
    if (top_k == 0)    top_k = 8;
    if (fts_pool == 0) fts_pool = 32;
    std::vector<Hit> out;

    if (scope == Scope::Project) {
        auto fts = project_memory::search(query, fts_pool);
        for (const auto & f : fts) {
            Hit h;
            h.entry_id   = f.entry.id;
            h.content    = f.entry.content;
            h.category   = f.entry.category;
            h.tags       = f.entry.tags;
            h.fts_score  = f.score;
            h.snippet    = f.snippet;
            h.trigram    = merge_dedup::trigram_jaccard(query, f.entry.content);
            // bm25 is lower-is-better; convert with a soft inversion.
            double bm_norm = 1.0 / (1.0 + (f.score < 0 ? -f.score : f.score));
            h.combined   = 0.7 * bm_norm + 0.3 * h.trigram;
            h.gloss      = build_gloss(query, f.entry.content, f.score, h.trigram);
            out.push_back(std::move(h));
        }
    } else {
        auto fts = personal_memory::search(query, fts_pool);
        for (const auto & f : fts) {
            Hit h;
            h.entry_id   = f.entry.id;
            h.content    = f.entry.content;
            h.category   = f.entry.category;
            h.tags       = f.entry.tags;
            h.fts_score  = f.score;
            h.snippet    = f.snippet;
            h.trigram    = merge_dedup::trigram_jaccard(query, f.entry.content);
            double bm_norm = 1.0 / (1.0 + (f.score < 0 ? -f.score : f.score));
            h.combined   = 0.7 * bm_norm + 0.3 * h.trigram;
            h.gloss      = build_gloss(query, f.entry.content, f.score, h.trigram);
            out.push_back(std::move(h));
        }
    }

    std::sort(out.begin(), out.end(),
              [](const Hit & a, const Hit & b){ return a.combined > b.combined; });
    if (out.size() > top_k) out.resize(top_k);
    return out;
}

}
