// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1915_magic_sheet_builder/magic_sheet_builder.hpp"

namespace {

testing::TestOutcome run_magic_sheet_builder() {
    magic_sheet_builder::init();
    auto s = magic_sheet_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    magic_sheet_builder::shutdown();
    return testing::ok();
}

const int _reg_magic_sheet_builder = testing::register_test(
    "magic_sheet_builder",
    "1915_magic_sheet_builder: stub status check",
    &run_magic_sheet_builder);

}
