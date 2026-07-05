// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1172_aircraft_logbook/aircraft_logbook.hpp"

namespace {

testing::TestOutcome run_aircraft_logbook() {
    aircraft_logbook::init();
    auto s = aircraft_logbook::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    aircraft_logbook::shutdown();
    return testing::ok();
}

const int _reg_aircraft_logbook = testing::register_test(
    "aircraft_logbook",
    "1172_aircraft_logbook: stub status check",
    &run_aircraft_logbook);

}
