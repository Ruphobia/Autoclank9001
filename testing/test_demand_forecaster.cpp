// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/481_demand_forecaster/demand_forecaster.hpp"

namespace {

testing::TestOutcome run_demand_forecaster() {
    demand_forecaster::init();
    auto s = demand_forecaster::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    demand_forecaster::shutdown();
    return testing::ok();
}

const int _reg_demand_forecaster = testing::register_test(
    "demand_forecaster",
    "481_demand_forecaster: stub status check",
    &run_demand_forecaster);

}
