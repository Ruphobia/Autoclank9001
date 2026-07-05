// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1885_population_and_census_tracker/population_and_census_tracker.hpp"

namespace {

testing::TestOutcome run_population_and_census_tracker() {
    population_and_census_tracker::init();
    auto s = population_and_census_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    population_and_census_tracker::shutdown();
    return testing::ok();
}

const int _reg_population_and_census_tracker = testing::register_test(
    "population_and_census_tracker",
    "1885_population_and_census_tracker: stub status check",
    &run_population_and_census_tracker);

}
