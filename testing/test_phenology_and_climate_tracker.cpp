// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1578_phenology_and_climate_tracker/phenology_and_climate_tracker.hpp"

namespace {

testing::TestOutcome run_phenology_and_climate_tracker() {
    phenology_and_climate_tracker::init();
    auto s = phenology_and_climate_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    phenology_and_climate_tracker::shutdown();
    return testing::ok();
}

const int _reg_phenology_and_climate_tracker = testing::register_test(
    "phenology_and_climate_tracker",
    "1578_phenology_and_climate_tracker: stub status check",
    &run_phenology_and_climate_tracker);

}
