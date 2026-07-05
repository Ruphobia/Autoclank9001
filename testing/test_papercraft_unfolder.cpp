// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1877_papercraft_unfolder/papercraft_unfolder.hpp"

namespace {

testing::TestOutcome run_papercraft_unfolder() {
    papercraft_unfolder::init();
    auto s = papercraft_unfolder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    papercraft_unfolder::shutdown();
    return testing::ok();
}

const int _reg_papercraft_unfolder = testing::register_test(
    "papercraft_unfolder",
    "1877_papercraft_unfolder: stub status check",
    &run_papercraft_unfolder);

}
