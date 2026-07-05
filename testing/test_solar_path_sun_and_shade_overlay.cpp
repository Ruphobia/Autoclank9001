// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2031_solar_path_sun_and_shade_overlay/solar_path_sun_and_shade_overlay.hpp"

namespace {

testing::TestOutcome run_solar_path_sun_and_shade_overlay() {
    solar_path_sun_and_shade_overlay::init();
    auto s = solar_path_sun_and_shade_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    solar_path_sun_and_shade_overlay::shutdown();
    return testing::ok();
}

const int _reg_solar_path_sun_and_shade_overlay = testing::register_test(
    "solar_path_sun_and_shade_overlay",
    "2031_solar_path_sun_and_shade_overlay: stub status check",
    &run_solar_path_sun_and_shade_overlay);

}
