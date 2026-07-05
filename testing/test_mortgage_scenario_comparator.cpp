// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/533_mortgage_scenario_comparator/mortgage_scenario_comparator.hpp"

namespace {

testing::TestOutcome run_mortgage_scenario_comparator() {
    mortgage_scenario_comparator::init();
    auto s = mortgage_scenario_comparator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mortgage_scenario_comparator::shutdown();
    return testing::ok();
}

const int _reg_mortgage_scenario_comparator = testing::register_test(
    "mortgage_scenario_comparator",
    "533_mortgage_scenario_comparator: stub status check",
    &run_mortgage_scenario_comparator);

}
