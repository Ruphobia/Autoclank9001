#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

// Link Graph Explorer: extract <a href="..."> from HTML, build an
// in-crawl link graph from a (page_url, html) corpus, expose the
// usual queries (in-/out-degree, orphans, broken-link candidates,
// BFS shortest path).
//
// URL resolution against the page base is conservative: absolute
// http(s) URLs stay as-is, scheme-relative ("//host/path") are
// promoted to https, root-absolute ("/path") swap onto the page
// host, fragments and javascript: / mailto: hrefs are dropped.
namespace link_graph {

struct LinkRef {
    std::string href;          // raw href as the page wrote it
    std::string absolute;      // resolved against base_url, may be empty
    std::string anchor_text;
};

struct Page {
    std::string url;
    std::string html;
};

struct Graph {
    std::vector<std::string>                 nodes;      // page URLs, sorted
    std::vector<std::pair<std::string,std::string>> edges; // from -> to
};

void init();
void shutdown();

// Pull every <a href="..."> link from `html`, with the resolved
// absolute URL where possible and the visible anchor text.
std::vector<LinkRef> extract_links(std::string_view html,
                                   std::string_view base_url);

Graph build(const std::vector<Page> & pages);

size_t in_degree (const Graph & g, std::string_view node);
size_t out_degree(const Graph & g, std::string_view node);

// Nodes whose in-degree is 0.
std::vector<std::string> orphans(const Graph & g);

// URLs referenced by some page in the corpus but not themselves
// present as nodes. Heuristic "broken link" set: a real crawl can
// recheck these against the HTTP layer.
std::vector<std::string> dead_links(const Graph & g);

// BFS shortest path between two nodes (directed; follows edges in
// their natural direction).
std::vector<std::string> shortest_path(const Graph & g,
                                       std::string_view from,
                                       std::string_view to);

}
