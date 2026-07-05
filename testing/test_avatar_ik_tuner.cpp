// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/325_avatar_ik_tuner/avatar_ik_tuner.hpp"

namespace {

testing::TestOutcome run_avatar_ik_tuner() {
    avatar_ik_tuner::init();
    auto s = avatar_ik_tuner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    avatar_ik_tuner::shutdown();
    return testing::ok();
}

const int _reg_avatar_ik_tuner = testing::register_test(
    "avatar_ik_tuner",
    "325_avatar_ik_tuner: stub status check",
    &run_avatar_ik_tuner);

}
