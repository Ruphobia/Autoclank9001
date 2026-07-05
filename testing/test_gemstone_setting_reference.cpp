// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1846_gemstone_setting_reference/gemstone_setting_reference.hpp"

namespace {

testing::TestOutcome run_gemstone_setting_reference() {
    gemstone_setting_reference::init();
    auto s = gemstone_setting_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gemstone_setting_reference::shutdown();
    return testing::ok();
}

const int _reg_gemstone_setting_reference = testing::register_test(
    "gemstone_setting_reference",
    "1846_gemstone_setting_reference: stub status check",
    &run_gemstone_setting_reference);

}
