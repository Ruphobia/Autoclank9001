// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1071_utility_gis_overlay/utility_gis_overlay.hpp"

namespace {

testing::TestOutcome run_utility_gis_overlay() {
    utility_gis_overlay::init();
    auto s = utility_gis_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    utility_gis_overlay::shutdown();
    return testing::ok();
}

const int _reg_utility_gis_overlay = testing::register_test(
    "utility_gis_overlay",
    "1071_utility_gis_overlay: stub status check",
    &run_utility_gis_overlay);

}
