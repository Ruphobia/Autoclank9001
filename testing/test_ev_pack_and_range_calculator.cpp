// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1222_ev_pack_and_range_calculator/ev_pack_and_range_calculator.hpp"

namespace {

testing::TestOutcome run_ev_pack_and_range_calculator() {
    ev_pack_and_range_calculator::init();
    auto s = ev_pack_and_range_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ev_pack_and_range_calculator::shutdown();
    return testing::ok();
}

const int _reg_ev_pack_and_range_calculator = testing::register_test(
    "ev_pack_and_range_calculator",
    "1222_ev_pack_and_range_calculator: stub status check",
    &run_ev_pack_and_range_calculator);

}
