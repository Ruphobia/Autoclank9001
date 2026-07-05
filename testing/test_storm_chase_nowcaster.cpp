// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1473_storm_chase_nowcaster/storm_chase_nowcaster.hpp"

namespace {

testing::TestOutcome run_storm_chase_nowcaster() {
    storm_chase_nowcaster::init();
    auto s = storm_chase_nowcaster::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    storm_chase_nowcaster::shutdown();
    return testing::ok();
}

const int _reg_storm_chase_nowcaster = testing::register_test(
    "storm_chase_nowcaster",
    "1473_storm_chase_nowcaster: stub status check",
    &run_storm_chase_nowcaster);

}
