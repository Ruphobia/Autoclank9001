// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1990_scenario_builder/scenario_builder.hpp"

namespace {

testing::TestOutcome run_scenario_builder() {
    scenario_builder::init();
    auto s = scenario_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scenario_builder::shutdown();
    return testing::ok();
}

const int _reg_scenario_builder = testing::register_test(
    "scenario_builder",
    "1990_scenario_builder: stub status check",
    &run_scenario_builder);

}
