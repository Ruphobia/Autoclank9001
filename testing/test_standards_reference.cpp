// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/958_standards_reference/standards_reference.hpp"

namespace {

testing::TestOutcome run_standards_reference() {
    standards_reference::init();
    auto s = standards_reference::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    standards_reference::shutdown();
    return testing::ok();
}

const int _reg_standards_reference = testing::register_test(
    "standards_reference",
    "958_standards_reference: stub status check",
    &run_standards_reference);

}
