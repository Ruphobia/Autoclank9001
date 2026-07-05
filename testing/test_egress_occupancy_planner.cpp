// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1048_egress_occupancy_planner/egress_occupancy_planner.hpp"

namespace {

testing::TestOutcome run_egress_occupancy_planner() {
    egress_occupancy_planner::init();
    auto s = egress_occupancy_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    egress_occupancy_planner::shutdown();
    return testing::ok();
}

const int _reg_egress_occupancy_planner = testing::register_test(
    "egress_occupancy_planner",
    "1048_egress_occupancy_planner: stub status check",
    &run_egress_occupancy_planner);

}
