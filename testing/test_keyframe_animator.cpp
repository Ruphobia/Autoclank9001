// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1684_keyframe_animator/keyframe_animator.hpp"

namespace {

testing::TestOutcome run_keyframe_animator() {
    keyframe_animator::init();
    auto s = keyframe_animator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    keyframe_animator::shutdown();
    return testing::ok();
}

const int _reg_keyframe_animator = testing::register_test(
    "keyframe_animator",
    "1684_keyframe_animator: stub status check",
    &run_keyframe_animator);

}
