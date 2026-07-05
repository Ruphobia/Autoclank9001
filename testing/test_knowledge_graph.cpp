// SPDX-License-Identifier: GPL-3.0-or-later
// Smoke test for 034_knowledge_graph.

#include "test_runner.hpp"
#include "../modules/034_knowledge_graph/knowledge_graph.hpp"
#include "../modules/022_project_memory/project_memory.hpp"
#include "../modules/033_people/people.hpp"

#include <cstdlib>
#include <filesystem>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_kg";
    return fs::path("/tmp") / "tool_test_kg";
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    project_memory::open(root.string());
    knowledge_graph::init();

    auto e1 = project_memory::add("fact",
        "Dispenser leaks below 12 psi.",
        {"dispenser","hardware"}, "manual");
    auto e2 = project_memory::add("fact",
        "Dispenser drift trend confirmed.",
        {"dispenser","observed"}, "manual");
    auto e3 = project_memory::add("fact",
        "Carrier gas is argon.",
        {"chemistry"}, "manual");
    auto e4 = project_memory::add("fact",
        "Jane Bauer signed off the layout.",
        {"layout"}, "manual");

    people::Person jb;
    jb.name = "Jane Bauer";
    jb.aliases = {"Bauer"};
    int64_t p1 = people::add(people::Scope::Project, jb, root.string());
    people::link_entry(people::Scope::Project, p1, e1, root.string());

    auto g = knowledge_graph::build_project(root.string());

    if (g.nodes.size() != 5) return testing::fail("node count != 5: " + std::to_string(g.nodes.size()));

    bool saw_shared = false, saw_mention = false, saw_linked = false;
    for (const auto & e : g.edges) {
        if (e.kind == knowledge_graph::EdgeKind::SharedTag &&
            e.label == "dispenser" &&
            ((e.a.id == e1 && e.b.id == e2) || (e.a.id == e2 && e.b.id == e1)))
            saw_shared = true;
        if (e.kind == knowledge_graph::EdgeKind::Mention &&
            e.a.kind == knowledge_graph::NodeKind::Person &&
            e.a.id == p1 && e.b.id == e4)
            saw_mention = true;
        if (e.kind == knowledge_graph::EdgeKind::LinkedEntry &&
            e.a.id == p1 && e.b.id == e1)
            saw_linked = true;
    }
    if (!saw_shared)  return testing::fail("expected shared-tag edge e1<->e2");
    if (!saw_mention) return testing::fail("expected mention edge person->e4");
    if (!saw_linked)  return testing::fail("expected linked-entry edge person->e1");

    // Orphans: e3 (carrier gas) has no shared tag and no person mention.
    auto orph = knowledge_graph::orphans(g);
    bool e3_orphan = false;
    for (const auto & n : orph) {
        if (n.kind == knowledge_graph::NodeKind::Entry && n.id == e3) e3_orphan = true;
    }
    if (!e3_orphan) return testing::fail("e3 should be an orphan");

    // Connected components: e3 alone; the others in one component.
    auto cc = knowledge_graph::connected_components(g);
    bool has_big = false, has_single = false;
    for (const auto & c : cc) {
        if (c.size() == 1 && c[0].kind == knowledge_graph::NodeKind::Entry && c[0].id == e3)
            has_single = true;
        if (c.size() >= 4) has_big = true;
    }
    if (!has_big || !has_single)
        return testing::fail("component shape wrong");

    // Shortest path from e2 to e4 via the dispenser tag, person, and link.
    auto path = knowledge_graph::shortest_path(g,
        knowledge_graph::NodeId{knowledge_graph::NodeKind::Entry, e2},
        knowledge_graph::NodeId{knowledge_graph::NodeKind::Entry, e4});
    if (path.empty()) return testing::fail("no path e2 -> e4");
    if (path.front().id != e2 || path.back().id != e4)
        return testing::fail("path endpoints wrong");

    knowledge_graph::shutdown();
    project_memory::close();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "knowledge_graph",
    "034_knowledge_graph: build + components + orphans + shortest path",
    &run);

}
