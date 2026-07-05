// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/924_ppe_selector/ppe_selector.hpp"

namespace {

testing::TestOutcome run_ppe_selector() {
    ppe_selector::init();
    auto s = ppe_selector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ppe_selector::shutdown();
    return testing::ok();
}

const int _reg_ppe_selector = testing::register_test(
    "ppe_selector",
    "924_ppe_selector: stub status check",
    &run_ppe_selector);

}
