// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/534_cap_rate_and_noi_calculator/cap_rate_and_noi_calculator.hpp"

namespace {

testing::TestOutcome run_cap_rate_and_noi_calculator() {
    cap_rate_and_noi_calculator::init();
    auto s = cap_rate_and_noi_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cap_rate_and_noi_calculator::shutdown();
    return testing::ok();
}

const int _reg_cap_rate_and_noi_calculator = testing::register_test(
    "cap_rate_and_noi_calculator",
    "534_cap_rate_and_noi_calculator: stub status check",
    &run_cap_rate_and_noi_calculator);

}
