// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2046_fish_id/fish_id.hpp"

namespace {

testing::TestOutcome run_fish_id() {
    fish_id::init();
    auto s = fish_id::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fish_id::shutdown();
    return testing::ok();
}

const int _reg_fish_id = testing::register_test(
    "fish_id",
    "2046_fish_id: stub status check",
    &run_fish_id);

}
