// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1658_xyz_plot_and_grid/xyz_plot_and_grid.hpp"

namespace {

testing::TestOutcome run_xyz_plot_and_grid() {
    xyz_plot_and_grid::init();
    auto s = xyz_plot_and_grid::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    xyz_plot_and_grid::shutdown();
    return testing::ok();
}

const int _reg_xyz_plot_and_grid = testing::register_test(
    "xyz_plot_and_grid",
    "1658_xyz_plot_and_grid: stub status check",
    &run_xyz_plot_and_grid);

}
