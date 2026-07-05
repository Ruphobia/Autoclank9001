// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/524_classroom_runner/classroom_runner.hpp"

namespace {

testing::TestOutcome run_classroom_runner() {
    classroom_runner::init();
    auto s = classroom_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    classroom_runner::shutdown();
    return testing::ok();
}

const int _reg_classroom_runner = testing::register_test(
    "classroom_runner",
    "524_classroom_runner: stub status check",
    &run_classroom_runner);

}
