// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1283_off_grid_system_sizer/off_grid_system_sizer.hpp"

namespace {

testing::TestOutcome run_off_grid_system_sizer() {
    off_grid_system_sizer::init();
    auto s = off_grid_system_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    off_grid_system_sizer::shutdown();
    return testing::ok();
}

const int _reg_off_grid_system_sizer = testing::register_test(
    "off_grid_system_sizer",
    "1283_off_grid_system_sizer: stub status check",
    &run_off_grid_system_sizer);

}
