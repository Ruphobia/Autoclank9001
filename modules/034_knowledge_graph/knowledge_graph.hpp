#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Knowledge Graph View: build a graph over the project memory entries
// and people, with edges for shared tags, person mentions, and the
// explicit person->entry links the People module records. Then expose
// the usual graph queries: connected components, orphans (nodes with
// no edges), and shortest path between two nodes (unweighted BFS).
//
// The graph is recomputed from the underlying stores on demand. There
// is no separate persisted graph file; this is a view, not a third
// source of truth.
namespace knowledge_graph {

enum class NodeKind { Entry = 0, Person = 1 };
const char * to_string(NodeKind);

struct NodeId {
    NodeKind kind;
    int64_t  id;
    bool operator==(const NodeId & o) const { return kind == o.kind && id == o.id; }
};

enum class EdgeKind { SharedTag = 0, Mention = 1, LinkedEntry = 2 };
const char * to_string(EdgeKind);

struct Edge {
    NodeId      a;
    NodeId      b;
    EdgeKind    kind;
    std::string label;          // e.g. the tag name for SharedTag
};

struct Node {
    NodeId      id;
    std::string title;          // entry: first 64 chars of content; person: name
    std::vector<std::string> tags;
};

struct Graph {
    std::vector<Node> nodes;
    std::vector<Edge> edges;
};

void init();
void shutdown();

// Build the graph from the project-scope stores. Reads
// 022_project_memory entries plus 033_people Project-scope records;
// emits one node per entry and per person. Edges:
//   SharedTag    - between two entries that share at least one tag (label = tag name)
//   Mention      - between a person and an entry whose content names them
//   LinkedEntry  - between a person and an entry in their linked_entries list
Graph build_project(std::string_view project_root);

// Unweighted shortest path between two nodes. Returns the node sequence
// from `from` to `to`, inclusive. Empty if no path exists.
std::vector<NodeId> shortest_path(const Graph & g,
                                  const NodeId & from,
                                  const NodeId & to);

// Connected components (treating edges as undirected). Returns one
// vector per component, smallest-id first within each component.
std::vector<std::vector<NodeId>> connected_components(const Graph & g);

// Nodes with no edges incident to them.
std::vector<NodeId> orphans(const Graph & g);

}
