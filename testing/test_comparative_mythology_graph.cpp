// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/744_comparative_mythology_graph/comparative_mythology_graph.hpp"

namespace {

testing::TestOutcome run_comparative_mythology_graph() {
    comparative_mythology_graph::init();
    auto s = comparative_mythology_graph::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comparative_mythology_graph::shutdown();
    return testing::ok();
}

const int _reg_comparative_mythology_graph = testing::register_test(
    "comparative_mythology_graph",
    "744_comparative_mythology_graph: stub status check",
    &run_comparative_mythology_graph);

}
