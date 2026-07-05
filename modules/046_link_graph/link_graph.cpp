#include "link_graph.hpp"

#include <algorithm>
#include <cctype>
#include <deque>
#include <regex>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace link_graph {
namespace {

std::string strip_tags_short(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    bool in_tag = false;
    for (char c : s) {
        if (c == '<') in_tag = true;
        else if (c == '>') { in_tag = false; out.push_back(' '); }
        else if (!in_tag) out.push_back(c);
    }
    // Trim and collapse whitespace.
    std::string collapsed;
    bool ws = false;
    for (char c : out) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!ws) { collapsed.push_back(' '); ws = true; }
        } else { collapsed.push_back(c); ws = false; }
    }
    size_t a = 0, b = collapsed.size();
    while (a < b && std::isspace(static_cast<unsigned char>(collapsed[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(collapsed[b - 1]))) --b;
    return collapsed.substr(a, b - a);
}

struct BaseParts {
    std::string scheme;
    std::string host;
};
BaseParts base_of(std::string_view base) {
    BaseParts p;
    auto colon = base.find(":");
    if (colon == std::string_view::npos) return p;
    p.scheme.assign(base.begin(), base.begin() + colon);
    if (base.size() < colon + 3 || base[colon+1] != '/' || base[colon+2] != '/') return p;
    auto after = base.substr(colon + 3);
    auto end = after.find_first_of("/?#");
    if (end == std::string_view::npos) p.host.assign(after.begin(), after.end());
    else                                p.host.assign(after.begin(), after.begin() + end);
    return p;
}

std::string resolve(std::string_view href, std::string_view base_url) {
    if (href.empty()) return {};
    if (href.compare(0, 11, "javascript:") == 0) return {};
    if (href.compare(0, 7,  "mailto:")     == 0) return {};
    if (href.compare(0, 4,  "tel:")        == 0) return {};
    // Drop fragment.
    std::string h{href};
    auto hash = h.find('#');
    if (hash != std::string::npos) h.erase(hash);
    if (h.empty()) return {};

    if (h.rfind("http://", 0) == 0 || h.rfind("https://", 0) == 0) return h;
    if (h.rfind("//", 0) == 0) return "https:" + h;     // scheme-relative
    auto b = base_of(base_url);
    if (b.host.empty()) return {};
    std::string scheme = b.scheme.empty() ? "https" : b.scheme;
    if (h.front() == '/') return scheme + "://" + b.host + h;
    // Relative path: stitch onto base URL's directory.
    std::string s{base_url};
    auto last = s.find_last_of('/');
    if (last == std::string::npos || last < scheme.size() + 3) {
        return scheme + "://" + b.host + "/" + h;
    }
    return s.substr(0, last + 1) + h;
}

}

void init()     {}
void shutdown() {}

std::vector<LinkRef> extract_links(std::string_view html_v, std::string_view base_url) {
    std::string html{html_v};
    std::vector<LinkRef> out;
    static const std::regex re_a(
        R"(<a[^>]*href=\"([^\"]*)\"[^>]*>([\s\S]*?)</a>)",
        std::regex::icase);
    for (auto it = std::sregex_iterator(html.begin(), html.end(), re_a);
         it != std::sregex_iterator(); ++it) {
        LinkRef l;
        l.href        = (*it)[1].str();
        l.absolute    = resolve(l.href, base_url);
        l.anchor_text = strip_tags_short((*it)[2].str());
        out.push_back(l);
    }
    return out;
}

Graph build(const std::vector<Page> & pages) {
    Graph g;
    std::set<std::string> nodes;
    for (const auto & p : pages) nodes.insert(p.url);
    g.nodes.assign(nodes.begin(), nodes.end());

    for (const auto & p : pages) {
        auto links = extract_links(p.html, p.url);
        for (const auto & l : links) {
            if (l.absolute.empty()) continue;
            g.edges.emplace_back(p.url, l.absolute);
        }
    }
    std::sort(g.edges.begin(), g.edges.end());
    g.edges.erase(std::unique(g.edges.begin(), g.edges.end()), g.edges.end());
    return g;
}

size_t in_degree(const Graph & g, std::string_view node) {
    size_t n = 0;
    for (const auto & e : g.edges) if (e.second == node) ++n;
    return n;
}
size_t out_degree(const Graph & g, std::string_view node) {
    size_t n = 0;
    for (const auto & e : g.edges) if (e.first == node) ++n;
    return n;
}

std::vector<std::string> orphans(const Graph & g) {
    std::unordered_set<std::string> targets;
    for (const auto & e : g.edges) targets.insert(e.second);
    std::vector<std::string> out;
    for (const auto & n : g.nodes) if (!targets.count(n)) out.push_back(n);
    return out;
}

std::vector<std::string> dead_links(const Graph & g) {
    std::unordered_set<std::string> have(g.nodes.begin(), g.nodes.end());
    std::set<std::string> dead;
    for (const auto & e : g.edges) if (!have.count(e.second)) dead.insert(e.second);
    return std::vector<std::string>(dead.begin(), dead.end());
}

std::vector<std::string> shortest_path(const Graph & g,
                                       std::string_view from_v, std::string_view to_v) {
    std::string from{from_v}, to{to_v};
    std::unordered_map<std::string, std::vector<std::string>> adj;
    std::unordered_set<std::string> all;
    for (const auto & n : g.nodes) { adj[n]; all.insert(n); }
    for (const auto & e : g.edges) {
        adj[e.first].push_back(e.second);
        all.insert(e.first); all.insert(e.second);
    }
    if (!all.count(from) || !all.count(to)) return {};
    if (from == to) return {from};
    std::unordered_map<std::string, std::string> back;
    std::deque<std::string> q;
    q.push_back(from); back[from] = from;
    while (!q.empty()) {
        std::string u = q.front(); q.pop_front();
        if (u == to) break;
        for (const auto & v : adj[u]) {
            if (back.count(v)) continue;
            back[v] = u;
            q.push_back(v);
        }
    }
    if (!back.count(to)) return {};
    std::vector<std::string> rev;
    for (std::string cur = to;; cur = back[cur]) {
        rev.push_back(cur);
        if (cur == back[cur]) break;
    }
    std::reverse(rev.begin(), rev.end());
    return rev;
}

}
