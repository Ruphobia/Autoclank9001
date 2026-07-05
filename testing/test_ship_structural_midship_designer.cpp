// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1079_ship_structural_midship_designer/ship_structural_midship_designer.hpp"

namespace {

testing::TestOutcome run_ship_structural_midship_designer() {
    ship_structural_midship_designer::init();
    auto s = ship_structural_midship_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ship_structural_midship_designer::shutdown();
    return testing::ok();
}

const int _reg_ship_structural_midship_designer = testing::register_test(
    "ship_structural_midship_designer",
    "1079_ship_structural_midship_designer: stub status check",
    &run_ship_structural_midship_designer);

}
