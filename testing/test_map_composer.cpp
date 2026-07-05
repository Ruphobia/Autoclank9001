// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1454_map_composer/map_composer.hpp"

namespace {

testing::TestOutcome run_map_composer() {
    map_composer::init();
    auto s = map_composer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    map_composer::shutdown();
    return testing::ok();
}

const int _reg_map_composer = testing::register_test(
    "map_composer",
    "1454_map_composer: stub status check",
    &run_map_composer);

}
