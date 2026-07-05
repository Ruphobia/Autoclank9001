// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1181_repeater_and_satellite_database/repeater_and_satellite_database.hpp"

namespace {

testing::TestOutcome run_repeater_and_satellite_database() {
    repeater_and_satellite_database::init();
    auto s = repeater_and_satellite_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    repeater_and_satellite_database::shutdown();
    return testing::ok();
}

const int _reg_repeater_and_satellite_database = testing::register_test(
    "repeater_and_satellite_database",
    "1181_repeater_and_satellite_database: stub status check",
    &run_repeater_and_satellite_database);

}
