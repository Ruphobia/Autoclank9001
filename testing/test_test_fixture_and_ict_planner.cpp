// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/866_test_fixture_and_ict_planner/test_fixture_and_ict_planner.hpp"

namespace {

testing::TestOutcome run_test_fixture_and_ict_planner() {
    test_fixture_and_ict_planner::init();
    auto s = test_fixture_and_ict_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    test_fixture_and_ict_planner::shutdown();
    return testing::ok();
}

const int _reg_test_fixture_and_ict_planner = testing::register_test(
    "test_fixture_and_ict_planner",
    "866_test_fixture_and_ict_planner: stub status check",
    &run_test_fixture_and_ict_planner);

}
