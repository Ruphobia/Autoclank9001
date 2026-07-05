// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/999_machinability_and_speeds_lookup/machinability_and_speeds_lookup.hpp"

namespace {

testing::TestOutcome run_machinability_and_speeds_lookup() {
    machinability_and_speeds_lookup::init();
    auto s = machinability_and_speeds_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    machinability_and_speeds_lookup::shutdown();
    return testing::ok();
}

const int _reg_machinability_and_speeds_lookup = testing::register_test(
    "machinability_and_speeds_lookup",
    "999_machinability_and_speeds_lookup: stub status check",
    &run_machinability_and_speeds_lookup);

}
