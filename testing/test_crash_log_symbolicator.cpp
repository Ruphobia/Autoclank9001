// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/164_crash_log_symbolicator/crash_log_symbolicator.hpp"

namespace {

testing::TestOutcome run_crash_log_symbolicator() {
    crash_log_symbolicator::init();
    auto s = crash_log_symbolicator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crash_log_symbolicator::shutdown();
    return testing::ok();
}

const int _reg_crash_log_symbolicator = testing::register_test(
    "crash_log_symbolicator",
    "164_crash_log_symbolicator: stub status check",
    &run_crash_log_symbolicator);

}
