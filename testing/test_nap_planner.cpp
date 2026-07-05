// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2163_nap_planner/nap_planner.hpp"

namespace {

testing::TestOutcome run_nap_planner() {
    nap_planner::init();
    auto s = nap_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nap_planner::shutdown();
    return testing::ok();
}

const int _reg_nap_planner = testing::register_test(
    "nap_planner",
    "2163_nap_planner: stub status check",
    &run_nap_planner);

}
