// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/782_genealogy_graph/genealogy_graph.hpp"

namespace {

testing::TestOutcome run_genealogy_graph() {
    genealogy_graph::init();
    auto s = genealogy_graph::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    genealogy_graph::shutdown();
    return testing::ok();
}

const int _reg_genealogy_graph = testing::register_test(
    "genealogy_graph",
    "782_genealogy_graph: stub status check",
    &run_genealogy_graph);

}
