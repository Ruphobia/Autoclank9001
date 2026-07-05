// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1659_style_transfer/style_transfer.hpp"

namespace {

testing::TestOutcome run_style_transfer() {
    style_transfer::init();
    auto s = style_transfer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    style_transfer::shutdown();
    return testing::ok();
}

const int _reg_style_transfer = testing::register_test(
    "style_transfer",
    "1659_style_transfer: stub status check",
    &run_style_transfer);

}
