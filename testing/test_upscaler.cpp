// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1626_upscaler/upscaler.hpp"

namespace {

testing::TestOutcome run_upscaler() {
    upscaler::init();
    auto s = upscaler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    upscaler::shutdown();
    return testing::ok();
}

const int _reg_upscaler = testing::register_test(
    "upscaler",
    "1626_upscaler: stub status check",
    &run_upscaler);

}
