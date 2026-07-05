// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1180_callsign_lookup/callsign_lookup.hpp"

namespace {

testing::TestOutcome run_callsign_lookup() {
    callsign_lookup::init();
    auto s = callsign_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    callsign_lookup::shutdown();
    return testing::ok();
}

const int _reg_callsign_lookup = testing::register_test(
    "callsign_lookup",
    "1180_callsign_lookup: stub status check",
    &run_callsign_lookup);

}
