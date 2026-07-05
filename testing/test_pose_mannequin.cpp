// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1872_pose_mannequin/pose_mannequin.hpp"

namespace {

testing::TestOutcome run_pose_mannequin() {
    pose_mannequin::init();
    auto s = pose_mannequin::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pose_mannequin::shutdown();
    return testing::ok();
}

const int _reg_pose_mannequin = testing::register_test(
    "pose_mannequin",
    "1872_pose_mannequin: stub status check",
    &run_pose_mannequin);

}
