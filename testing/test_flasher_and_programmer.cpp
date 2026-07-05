// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/183_flasher_and_programmer/flasher_and_programmer.hpp"

namespace {

testing::TestOutcome run_flasher_and_programmer() {
    flasher_and_programmer::init();
    auto s = flasher_and_programmer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    flasher_and_programmer::shutdown();
    return testing::ok();
}

const int _reg_flasher_and_programmer = testing::register_test(
    "flasher_and_programmer",
    "183_flasher_and_programmer: stub status check",
    &run_flasher_and_programmer);

}
