// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 046_link_graph.

#include "test_runner.hpp"
#include "../modules/046_link_graph/link_graph.hpp"

namespace {

testing::TestOutcome run() {
    link_graph::init();

    // extract_links resolves three flavours: absolute, root-absolute,
    // and a javascript: / mailto: drop.
    std::string html = R"HTML(
<html><body>
  <a href="https://other.com/a">other</a>
  <a href="/inner">inner</a>
  <a href="//cdn.example.com/x">cdn</a>
  <a href="javascript:foo()">skip</a>
  <a href="mailto:x@example.com">skip mail</a>
  <a href="#frag">just fragment</a>
</body></html>
)HTML";
    auto links = link_graph::extract_links(html, "https://example.com/start");
    if (links.size() != 6) return testing::fail("anchor count != 6");

    bool saw_other = false, saw_inner = false, saw_cdn = false;
    for (const auto & l : links) {
        if (l.absolute == "https://other.com/a") saw_other = true;
        if (l.absolute == "https://example.com/inner") saw_inner = true;
        if (l.absolute == "https://cdn.example.com/x") saw_cdn = true;
        if (l.href.rfind("javascript:", 0) == 0 && !l.absolute.empty())
            return testing::fail("javascript: not dropped");
        if (l.href.rfind("mailto:", 0) == 0 && !l.absolute.empty())
            return testing::fail("mailto: not dropped");
        if (l.href == "#frag" && !l.absolute.empty())
            return testing::fail("fragment-only not dropped");
    }
    if (!saw_other || !saw_inner || !saw_cdn)
        return testing::fail("link resolution miss");

    // build over a small corpus.
    std::vector<link_graph::Page> pages = {
        {"https://example.com/a",
         R"(<a href="/b">b</a> <a href="/c">c</a>)"},
        {"https://example.com/b",
         R"(<a href="/c">c</a>)"},
        {"https://example.com/c",
         R"(<a href="https://example.com/missing">missing</a>)"},
    };
    auto g = link_graph::build(pages);
    if (g.nodes.size() != 3) return testing::fail("node count != 3");

    if (link_graph::in_degree (g, "https://example.com/c") != 2)
        return testing::fail("in_degree(c) != 2");
    if (link_graph::out_degree(g, "https://example.com/a") != 2)
        return testing::fail("out_degree(a) != 2");

    auto orph = link_graph::orphans(g);
    if (orph.size() != 1 || orph[0] != "https://example.com/a")
        return testing::fail("orphan set wrong");

    auto dead = link_graph::dead_links(g);
    bool has_missing = false;
    for (const auto & d : dead) if (d == "https://example.com/missing") has_missing = true;
    if (!has_missing) return testing::fail("dead link not surfaced");

    auto path = link_graph::shortest_path(g,
        "https://example.com/a", "https://example.com/c");
    if (path.size() != 2 || path.front() != "https://example.com/a" ||
        path.back() != "https://example.com/c")
        return testing::fail("shortest path wrong (expected length 2)");

    link_graph::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "link_graph",
    "046_link_graph: anchor extraction + graph build + degree + orphans + dead links",
    &run);

}
