// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1821_espressoshotlog/espressoshotlog.hpp"

namespace {

testing::TestOutcome run_espressoshotlog() {
    espressoshotlog::init();
    auto s = espressoshotlog::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    espressoshotlog::shutdown();
    return testing::ok();
}

const int _reg_espressoshotlog = testing::register_test(
    "espressoshotlog",
    "1821_espressoshotlog: stub status check",
    &run_espressoshotlog);

}
