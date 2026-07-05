// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1249_trajectory_designer/trajectory_designer.hpp"

namespace {

testing::TestOutcome run_trajectory_designer() {
    trajectory_designer::init();
    auto s = trajectory_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trajectory_designer::shutdown();
    return testing::ok();
}

const int _reg_trajectory_designer = testing::register_test(
    "trajectory_designer",
    "1249_trajectory_designer: stub status check",
    &run_trajectory_designer);

}
