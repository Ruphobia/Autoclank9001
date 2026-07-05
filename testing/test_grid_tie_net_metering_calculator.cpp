// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1287_grid_tie_net_metering_calculator/grid_tie_net_metering_calculator.hpp"

namespace {

testing::TestOutcome run_grid_tie_net_metering_calculator() {
    grid_tie_net_metering_calculator::init();
    auto s = grid_tie_net_metering_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    grid_tie_net_metering_calculator::shutdown();
    return testing::ok();
}

const int _reg_grid_tie_net_metering_calculator = testing::register_test(
    "grid_tie_net_metering_calculator",
    "1287_grid_tie_net_metering_calculator: stub status check",
    &run_grid_tie_net_metering_calculator);

}
