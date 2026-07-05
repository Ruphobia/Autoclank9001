// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1471_reprojection_regrid_kitchen/reprojection_regrid_kitchen.hpp"

namespace {

testing::TestOutcome run_reprojection_regrid_kitchen() {
    reprojection_regrid_kitchen::init();
    auto s = reprojection_regrid_kitchen::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reprojection_regrid_kitchen::shutdown();
    return testing::ok();
}

const int _reg_reprojection_regrid_kitchen = testing::register_test(
    "reprojection_regrid_kitchen",
    "1471_reprojection_regrid_kitchen: stub status check",
    &run_reprojection_regrid_kitchen);

}
