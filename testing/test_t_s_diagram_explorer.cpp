// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1543_t_s_diagram_explorer/t_s_diagram_explorer.hpp"

namespace {

testing::TestOutcome run_t_s_diagram_explorer() {
    t_s_diagram_explorer::init();
    auto s = t_s_diagram_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    t_s_diagram_explorer::shutdown();
    return testing::ok();
}

const int _reg_t_s_diagram_explorer = testing::register_test(
    "t_s_diagram_explorer",
    "1543_t_s_diagram_explorer: stub status check",
    &run_t_s_diagram_explorer);

}
