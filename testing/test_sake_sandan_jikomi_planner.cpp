// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1793_sake_sandan_jikomi_planner/sake_sandan_jikomi_planner.hpp"

namespace {

testing::TestOutcome run_sake_sandan_jikomi_planner() {
    sake_sandan_jikomi_planner::init();
    auto s = sake_sandan_jikomi_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sake_sandan_jikomi_planner::shutdown();
    return testing::ok();
}

const int _reg_sake_sandan_jikomi_planner = testing::register_test(
    "sake_sandan_jikomi_planner",
    "1793_sake_sandan_jikomi_planner: stub status check",
    &run_sake_sandan_jikomi_planner);

}
