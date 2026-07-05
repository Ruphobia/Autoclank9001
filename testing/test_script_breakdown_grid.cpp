// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1923_script_breakdown_grid/script_breakdown_grid.hpp"

namespace {

testing::TestOutcome run_script_breakdown_grid() {
    script_breakdown_grid::init();
    auto s = script_breakdown_grid::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    script_breakdown_grid::shutdown();
    return testing::ok();
}

const int _reg_script_breakdown_grid = testing::register_test(
    "script_breakdown_grid",
    "1923_script_breakdown_grid: stub status check",
    &run_script_breakdown_grid);

}
