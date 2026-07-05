// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/694_map_georectifier/map_georectifier.hpp"

namespace {

testing::TestOutcome run_map_georectifier() {
    map_georectifier::init();
    auto s = map_georectifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    map_georectifier::shutdown();
    return testing::ok();
}

const int _reg_map_georectifier = testing::register_test(
    "map_georectifier",
    "694_map_georectifier: stub status check",
    &run_map_georectifier);

}
