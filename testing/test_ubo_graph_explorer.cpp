// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/609_ubo_graph_explorer/ubo_graph_explorer.hpp"

namespace {

testing::TestOutcome run_ubo_graph_explorer() {
    ubo_graph_explorer::init();
    auto s = ubo_graph_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ubo_graph_explorer::shutdown();
    return testing::ok();
}

const int _reg_ubo_graph_explorer = testing::register_test(
    "ubo_graph_explorer",
    "609_ubo_graph_explorer: stub status check",
    &run_ubo_graph_explorer);

}
