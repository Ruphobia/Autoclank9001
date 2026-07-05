// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/941_sheet_metal_unfolder/sheet_metal_unfolder.hpp"

namespace {

testing::TestOutcome run_sheet_metal_unfolder() {
    sheet_metal_unfolder::init();
    auto s = sheet_metal_unfolder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sheet_metal_unfolder::shutdown();
    return testing::ok();
}

const int _reg_sheet_metal_unfolder = testing::register_test(
    "sheet_metal_unfolder",
    "941_sheet_metal_unfolder: stub status check",
    &run_sheet_metal_unfolder);

}
