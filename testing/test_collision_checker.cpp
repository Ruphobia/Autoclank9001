// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1251_collision_checker/collision_checker.hpp"

namespace {

testing::TestOutcome run_collision_checker() {
    collision_checker::init();
    auto s = collision_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    collision_checker::shutdown();
    return testing::ok();
}

const int _reg_collision_checker = testing::register_test(
    "collision_checker",
    "1251_collision_checker: stub status check",
    &run_collision_checker);

}
