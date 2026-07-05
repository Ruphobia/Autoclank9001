// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/896_plotter_2d_3d/plotter_2d_3d.hpp"

namespace {

testing::TestOutcome run_plotter_2d_3d() {
    plotter_2d_3d::init();
    auto s = plotter_2d_3d::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plotter_2d_3d::shutdown();
    return testing::ok();
}

const int _reg_plotter_2d_3d = testing::register_test(
    "plotter_2d_3d",
    "896_plotter_2d_3d: stub status check",
    &run_plotter_2d_3d);

}
