// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/200_hil_test_runner/hil_test_runner.hpp"

namespace {

testing::TestOutcome run_hil_test_runner() {
    hil_test_runner::init();
    auto s = hil_test_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hil_test_runner::shutdown();
    return testing::ok();
}

const int _reg_hil_test_runner = testing::register_test(
    "hil_test_runner",
    "200_hil_test_runner: stub status check",
    &run_hil_test_runner);

}
