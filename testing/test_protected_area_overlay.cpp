// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1573_protected_area_overlay/protected_area_overlay.hpp"

namespace {

testing::TestOutcome run_protected_area_overlay() {
    protected_area_overlay::init();
    auto s = protected_area_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    protected_area_overlay::shutdown();
    return testing::ok();
}

const int _reg_protected_area_overlay = testing::register_test(
    "protected_area_overlay",
    "1573_protected_area_overlay: stub status check",
    &run_protected_area_overlay);

}
