// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1558_nautical_chart_viewer/nautical_chart_viewer.hpp"

namespace {

testing::TestOutcome run_nautical_chart_viewer() {
    nautical_chart_viewer::init();
    auto s = nautical_chart_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nautical_chart_viewer::shutdown();
    return testing::ok();
}

const int _reg_nautical_chart_viewer = testing::register_test(
    "nautical_chart_viewer",
    "1558_nautical_chart_viewer: stub status check",
    &run_nautical_chart_viewer);

}
