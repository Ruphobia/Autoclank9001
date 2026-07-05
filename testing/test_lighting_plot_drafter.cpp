// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1912_lighting_plot_drafter/lighting_plot_drafter.hpp"

namespace {

testing::TestOutcome run_lighting_plot_drafter() {
    lighting_plot_drafter::init();
    auto s = lighting_plot_drafter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lighting_plot_drafter::shutdown();
    return testing::ok();
}

const int _reg_lighting_plot_drafter = testing::register_test(
    "lighting_plot_drafter",
    "1912_lighting_plot_drafter: stub status check",
    &run_lighting_plot_drafter);

}
