#include "knowledge_graph.hpp"

#include "../022_project_memory/project_memory.hpp"
#include "../033_people/people.hpp"

#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace knowledge_graph {
namespace {

struct IdKey {
    int kind;
    int64_t id;
    bool operator<(const IdKey & o) const {
        return kind < o.kind || (kind == o.kind && id < o.id);
    }
    bool operator==(const IdKey & o) const { return kind == o.kind && id == o.id; }
};
struct IdKeyHash {
    size_t operator()(const IdKey & k) const noexcept {
        return std::hash<int64_t>{}(((int64_t)k.kind << 56) ^ k.id);
    }
};

IdKey k_of(const NodeId & n) { return {(int)n.kind, n.id}; }

std::string truncated(const std::string & s, size_t n) {
    if (s.size() <= n) return s;
    std::string out = s.substr(0, n);
    out += "...";
    return out;
}

}

const char * to_string(NodeKind k) {
    return k == NodeKind::Entry ? "entry" : "person";
}
const char * to_string(EdgeKind k) {
    switch (k) {
        case EdgeKind::SharedTag:   return "shared-tag";
        case EdgeKind::Mention:     return "mention";
        case EdgeKind::LinkedEntry: return "linked-entry";
    }
    return "shared-tag";
}

void init()     {}
void shutdown() {}

Graph build_project(std::string_view project_root) {
    Graph g;

    auto entries = project_memory::list({}, {}, 10000);
    auto folks   = people::list(people::Scope::Project, project_root);

    // Nodes.
    for (const auto & e : entries) {
        Node n;
        n.id    = NodeId{NodeKind::Entry, e.id};
        n.title = truncated(e.content, 64);
        n.tags  = e.tags;
        g.nodes.push_back(n);
    }
    for (const auto & p : folks) {
        Node n;
        n.id    = NodeId{NodeKind::Person, p.id};
        n.title = p.name;
        g.nodes.push_back(n);
    }

    // SharedTag edges: build tag -> [entry id] inverted index, then
    // emit one edge per pair within each tag's bucket.
    std::unordered_map<std::string, std::vector<int64_t>> tag_to_entries;
    for (const auto & e : entries)
        for (const auto & t : e.tags) tag_to_entries[t].push_back(e.id);

    std::set<std::tuple<int64_t,int64_t,std::string>> dedup;
    for (const auto & [tag, ids] : tag_to_entries) {
        for (size_t i = 0; i < ids.size(); ++i) {
            for (size_t j = i + 1; j < ids.size(); ++j) {
                int64_t lo = std::min(ids[i], ids[j]);
                int64_t hi = std::max(ids[i], ids[j]);
                auto key = std::make_tuple(lo, hi, tag);
                if (!dedup.insert(key).second) continue;
                Edge e;
                e.a     = NodeId{NodeKind::Entry, lo};
                e.b     = NodeId{NodeKind::Entry, hi};
                e.kind  = EdgeKind::SharedTag;
                e.label = tag;
                g.edges.push_back(e);
            }
        }
    }

    // Mention edges: scan each entry's content for any person.
    for (const auto & e : entries) {
        auto m = people::find_mentions(people::Scope::Project, e.content, project_root);
        for (int64_t pid : m) {
            Edge edge;
            edge.a    = NodeId{NodeKind::Person, pid};
            edge.b    = NodeId{NodeKind::Entry, e.id};
            edge.kind = EdgeKind::Mention;
            g.edges.push_back(edge);
        }
    }

    // LinkedEntry edges from People records.
    for (const auto & p : folks) {
        for (int64_t eid : p.linked_entries) {
            Edge edge;
            edge.a    = NodeId{NodeKind::Person, p.id};
            edge.b    = NodeId{NodeKind::Entry, eid};
            edge.kind = EdgeKind::LinkedEntry;
            g.edges.push_back(edge);
        }
    }
    return g;
}

namespace {
std::unordered_map<IdKey, std::vector<IdKey>, IdKeyHash> build_adj(const Graph & g) {
    std::unordered_map<IdKey, std::vector<IdKey>, IdKeyHash> adj;
    for (const auto & n : g.nodes) adj[k_of(n.id)];
    for (const auto & e : g.edges) {
        adj[k_of(e.a)].push_back(k_of(e.b));
        adj[k_of(e.b)].push_back(k_of(e.a));
    }
    return adj;
}
NodeId from_key(const IdKey & k) {
    return NodeId{(NodeKind)k.kind, k.id};
}
}

std::vector<NodeId> shortest_path(const Graph & g,
                                  const NodeId & from, const NodeId & to) {
    auto adj = build_adj(g);
    IdKey s = k_of(from), t = k_of(to);
    if (!adj.count(s) || !adj.count(t)) return {};
    if (s == t) return {from};
    std::unordered_map<IdKey, IdKey, IdKeyHash> back;
    std::deque<IdKey> q;
    q.push_back(s); back[s] = s;
    while (!q.empty()) {
        IdKey u = q.front(); q.pop_front();
        if (u == t) break;
        for (const IdKey & v : adj[u]) {
            if (back.count(v)) continue;
            back[v] = u;
            q.push_back(v);
        }
    }
    if (!back.count(t)) return {};
    std::vector<NodeId> rev;
    for (IdKey cur = t;; cur = back[cur]) {
        rev.push_back(from_key(cur));
        if (cur == back[cur]) break;
    }
    std::reverse(rev.begin(), rev.end());
    return rev;
}

std::vector<std::vector<NodeId>> connected_components(const Graph & g) {
    auto adj = build_adj(g);
    std::unordered_set<IdKey, IdKeyHash> seen;
    std::vector<std::vector<NodeId>> out;
    for (const auto & n : g.nodes) {
        IdKey s = k_of(n.id);
        if (seen.count(s)) continue;
        std::vector<NodeId> comp;
        std::deque<IdKey> q;
        q.push_back(s); seen.insert(s);
        while (!q.empty()) {
            IdKey u = q.front(); q.pop_front();
            comp.push_back(from_key(u));
            for (const auto & v : adj[u]) {
                if (seen.insert(v).second) q.push_back(v);
            }
        }
        std::sort(comp.begin(), comp.end(),
                  [](const NodeId & a, const NodeId & b){
                      return k_of(a) < k_of(b);
                  });
        out.push_back(std::move(comp));
    }
    return out;
}

std::vector<NodeId> orphans(const Graph & g) {
    auto adj = build_adj(g);
    std::vector<NodeId> out;
    for (const auto & n : g.nodes) {
        if (adj[k_of(n.id)].empty()) out.push_back(n.id);
    }
    return out;
}

}
