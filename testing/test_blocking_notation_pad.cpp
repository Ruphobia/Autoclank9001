// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1922_blocking_notation_pad/blocking_notation_pad.hpp"

namespace {

testing::TestOutcome run_blocking_notation_pad() {
    blocking_notation_pad::init();
    auto s = blocking_notation_pad::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    blocking_notation_pad::shutdown();
    return testing::ok();
}

const int _reg_blocking_notation_pad = testing::register_test(
    "blocking_notation_pad",
    "1922_blocking_notation_pad: stub status check",
    &run_blocking_notation_pad);

}
