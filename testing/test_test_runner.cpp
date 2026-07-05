// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/090_test_runner/test_runner.hpp"

namespace {

testing::TestOutcome run_test_runner() {
    test_runner::init();
    auto s = test_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    test_runner::shutdown();
    return testing::ok();
}

const int _reg_test_runner = testing::register_test(
    "test_runner",
    "090_test_runner: stub status check",
    &run_test_runner);

}
