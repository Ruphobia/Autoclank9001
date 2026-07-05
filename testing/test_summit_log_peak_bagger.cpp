// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2067_summit_log_peak_bagger/summit_log_peak_bagger.hpp"

namespace {

testing::TestOutcome run_summit_log_peak_bagger() {
    summit_log_peak_bagger::init();
    auto s = summit_log_peak_bagger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    summit_log_peak_bagger::shutdown();
    return testing::ok();
}

const int _reg_summit_log_peak_bagger = testing::register_test(
    "summit_log_peak_bagger",
    "2067_summit_log_peak_bagger: stub status check",
    &run_summit_log_peak_bagger);

}
