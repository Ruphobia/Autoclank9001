// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2127_radio_link_planner/radio_link_planner.hpp"

namespace {

testing::TestOutcome run_radio_link_planner() {
    radio_link_planner::init();
    auto s = radio_link_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    radio_link_planner::shutdown();
    return testing::ok();
}

const int _reg_radio_link_planner = testing::register_test(
    "radio_link_planner",
    "2127_radio_link_planner: stub status check",
    &run_radio_link_planner);

}
