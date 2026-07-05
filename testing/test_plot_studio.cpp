// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/656_plot_studio/plot_studio.hpp"

namespace {

testing::TestOutcome run_plot_studio() {
    plot_studio::init();
    auto s = plot_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plot_studio::shutdown();
    return testing::ok();
}

const int _reg_plot_studio = testing::register_test(
    "plot_studio",
    "656_plot_studio: stub status check",
    &run_plot_studio);

}
