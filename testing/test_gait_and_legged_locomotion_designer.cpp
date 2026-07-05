// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1268_gait_and_legged_locomotion_designer/gait_and_legged_locomotion_designer.hpp"

namespace {

testing::TestOutcome run_gait_and_legged_locomotion_designer() {
    gait_and_legged_locomotion_designer::init();
    auto s = gait_and_legged_locomotion_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gait_and_legged_locomotion_designer::shutdown();
    return testing::ok();
}

const int _reg_gait_and_legged_locomotion_designer = testing::register_test(
    "gait_and_legged_locomotion_designer",
    "1268_gait_and_legged_locomotion_designer: stub status check",
    &run_gait_and_legged_locomotion_designer);

}
