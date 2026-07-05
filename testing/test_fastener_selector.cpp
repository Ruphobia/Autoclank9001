// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/923_fastener_selector/fastener_selector.hpp"

namespace {

testing::TestOutcome run_fastener_selector() {
    fastener_selector::init();
    auto s = fastener_selector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fastener_selector::shutdown();
    return testing::ok();
}

const int _reg_fastener_selector = testing::register_test(
    "fastener_selector",
    "923_fastener_selector: stub status check",
    &run_fastener_selector);

}
