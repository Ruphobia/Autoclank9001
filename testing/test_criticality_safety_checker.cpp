// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1106_criticality_safety_checker/criticality_safety_checker.hpp"

namespace {

testing::TestOutcome run_criticality_safety_checker() {
    criticality_safety_checker::init();
    auto s = criticality_safety_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    criticality_safety_checker::shutdown();
    return testing::ok();
}

const int _reg_criticality_safety_checker = testing::register_test(
    "criticality_safety_checker",
    "1106_criticality_safety_checker: stub status check",
    &run_criticality_safety_checker);

}
