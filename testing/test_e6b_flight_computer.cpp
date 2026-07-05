// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1175_e6b_flight_computer/e6b_flight_computer.hpp"

namespace {

testing::TestOutcome run_e6b_flight_computer() {
    e6b_flight_computer::init();
    auto s = e6b_flight_computer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    e6b_flight_computer::shutdown();
    return testing::ok();
}

const int _reg_e6b_flight_computer = testing::register_test(
    "e6b_flight_computer",
    "1175_e6b_flight_computer: stub status check",
    &run_e6b_flight_computer);

}
