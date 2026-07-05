// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/993_galvanic_corrosion_checker/galvanic_corrosion_checker.hpp"

namespace {

testing::TestOutcome run_galvanic_corrosion_checker() {
    galvanic_corrosion_checker::init();
    auto s = galvanic_corrosion_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    galvanic_corrosion_checker::shutdown();
    return testing::ok();
}

const int _reg_galvanic_corrosion_checker = testing::register_test(
    "galvanic_corrosion_checker",
    "993_galvanic_corrosion_checker: stub status check",
    &run_galvanic_corrosion_checker);

}
