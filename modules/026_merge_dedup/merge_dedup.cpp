#include "merge_dedup.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../023_personal_memory/personal_memory.hpp"
#include "../025_forget_redact/forget_redact.hpp"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace merge_dedup {
namespace {

std::string normalize(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    bool ws = false;
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!ws) { out.push_back(' '); ws = true; }
        } else {
            out.push_back((char)std::tolower((unsigned char)c));
            ws = false;
        }
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

std::unordered_set<std::string> trigrams(std::string_view s) {
    std::unordered_set<std::string> g;
    std::string n = normalize(s);
    if (n.size() < 3) return g;
    for (size_t i = 0; i + 3 <= n.size(); ++i) g.insert(n.substr(i, 3));
    return g;
}

double jaccard(const std::unordered_set<std::string> & a,
               const std::unordered_set<std::string> & b) {
    if (a.empty() && b.empty()) return 1.0;
    if (a.empty() || b.empty()) return 0.0;
    size_t inter = 0;
    const auto & small = a.size() < b.size() ? a : b;
    const auto & big   = a.size() < b.size() ? b : a;
    for (const auto & t : small) if (big.count(t)) ++inter;
    size_t un = a.size() + b.size() - inter;
    return un == 0 ? 0.0 : (double)inter / (double)un;
}

// Fetch a snapshot of (id, category, content, tags) from the live store.
struct Snap {
    int64_t                  id;
    std::string              category;
    std::string              content;
    std::vector<std::string> tags;
};

std::vector<Snap> snap_all(Scope scope, std::string_view filter_category) {
    std::vector<Snap> out;
    auto cat = std::string_view{};
    if (!filter_category.empty()) cat = filter_category;
    if (scope == Scope::Project) {
        auto v = project_memory::list(cat, {}, 1000);
        for (auto & e : v) out.push_back({e.id, e.category, e.content, e.tags});
    } else {
        auto v = personal_memory::list(cat, {}, 1000);
        for (auto & e : v) out.push_back({e.id, e.category, e.content, e.tags});
    }
    return out;
}

std::optional<Snap> snap_one(Scope scope, int64_t id) {
    if (scope == Scope::Project) {
        auto e = project_memory::get(id);
        if (!e) return std::nullopt;
        return Snap{e->id, e->category, e->content, e->tags};
    }
    auto e = personal_memory::get(id);
    if (!e) return std::nullopt;
    return Snap{e->id, e->category, e->content, e->tags};
}

std::vector<std::string> split_sentences(const std::string & body) {
    std::vector<std::string> out;
    std::string cur;
    for (size_t i = 0; i < body.size(); ++i) {
        cur.push_back(body[i]);
        char c = body[i];
        if (c == '.' || c == '!' || c == '?' || c == '\n') {
            std::string s = cur;
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
            if (!s.empty()) out.push_back(s);
            cur.clear();
        }
    }
    if (!cur.empty()) {
        std::string s = cur;
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
        if (!s.empty()) out.push_back(s);
    }
    return out;
}

}

double trigram_jaccard(std::string_view a, std::string_view b) {
    return jaccard(trigrams(a), trigrams(b));
}

std::vector<DuplicateGroup>
    find_duplicates(Scope scope, std::string_view project_root,
                    double threshold, std::string_view category) {
    (void)project_root;
    std::vector<DuplicateGroup> out;
    auto entries = snap_all(scope, category);
    if (entries.size() < 2) return out;

    std::vector<std::unordered_set<std::string>> grams;
    grams.reserve(entries.size());
    for (const auto & e : entries) grams.push_back(trigrams(e.content));

    // Union-find via parent array.
    std::vector<size_t> parent(entries.size());
    for (size_t i = 0; i < parent.size(); ++i) parent[i] = i;
    auto find = [&](auto && self, size_t x) -> size_t {
        return parent[x] == x ? x : (parent[x] = self(self, parent[x]));
    };
    std::unordered_map<size_t, double> sim_sum;
    std::unordered_map<size_t, size_t> sim_pairs;

    for (size_t i = 0; i < entries.size(); ++i) {
        for (size_t j = i + 1; j < entries.size(); ++j) {
            double s = jaccard(grams[i], grams[j]);
            if (s < threshold) continue;
            size_t ri = find(find, i), rj = find(find, j);
            if (ri != rj) parent[ri] = rj;
            size_t r = find(find, j);
            sim_sum[r]   += s;
            sim_pairs[r] += 1;
        }
    }
    std::unordered_map<size_t, std::vector<int64_t>> groups;
    for (size_t i = 0; i < entries.size(); ++i) {
        size_t r = find(find, i);
        groups[r].push_back(entries[i].id);
    }
    for (auto & [r, ids] : groups) {
        if (ids.size() < 2) continue;
        DuplicateGroup g;
        g.ids = ids;
        std::sort(g.ids.begin(), g.ids.end());
        size_t pairs = sim_pairs[r];
        g.similarity = pairs ? sim_sum[r] / (double)pairs : 0.0;
        out.push_back(std::move(g));
    }
    std::sort(out.begin(), out.end(),
              [](const DuplicateGroup & a, const DuplicateGroup & b) {
                  return a.similarity > b.similarity;
              });
    return out;
}

MergedDraft propose_merge(Scope scope, const std::vector<int64_t> & ids,
                          std::string_view project_root) {
    (void)project_root;
    if (ids.size() < 2) throw std::runtime_error("propose_merge: need >= 2 ids");

    std::vector<Snap> snaps;
    snaps.reserve(ids.size());
    for (int64_t id : ids) {
        auto s = snap_one(scope, id);
        if (!s) throw std::runtime_error("propose_merge: missing live entry");
        snaps.push_back(*s);
    }

    MergedDraft d;
    d.merged_from = ids;
    d.category    = snaps.front().category;
    for (const auto & s : snaps) {
        if (s.category != d.category) {
            // Mixed categories: keep the first but flag it via a tag.
            d.tags.push_back("merge-mixed-category");
            break;
        }
    }
    std::set<std::string> tag_union;
    for (const auto & s : snaps) for (const auto & t : s.tags) tag_union.insert(t);
    for (const auto & t : tag_union) d.tags.push_back(t);

    std::set<std::string> seen_sentences;
    std::ostringstream    body;
    bool                  first = true;
    for (const auto & s : snaps) {
        for (const auto & sent : split_sentences(s.content)) {
            std::string key = normalize(sent);
            if (key.empty()) continue;
            if (!seen_sentences.insert(key).second) continue;
            if (!first) body << " ";
            body << sent;
            first = false;
        }
    }
    d.content = body.str();

    std::ostringstream src;
    src << "merge-dedup:from=";
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i) src << ",";
        src << ids[i];
    }
    d.source = src.str();
    return d;
}

int64_t apply_merge(Scope scope, const MergedDraft & draft,
                    std::string_view project_root) {
    int64_t new_id = 0;
    if (scope == Scope::Project)
        new_id = project_memory::add(draft.category, draft.content, draft.tags, draft.source);
    else
        new_id = personal_memory::add(draft.category, draft.content, draft.tags, draft.source);

    forget_redact::Scope fs_scope = (scope == Scope::Project)
        ? forget_redact::Scope::Project
        : forget_redact::Scope::Personal;
    std::ostringstream reason;
    reason << "merged into id=" << new_id;
    for (int64_t id : draft.merged_from) {
        forget_redact::soft_forget(fs_scope, id, reason.str(), project_root);
    }
    return new_id;
}

}
