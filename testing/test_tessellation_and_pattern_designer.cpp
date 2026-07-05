// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1878_tessellation_and_pattern_designer/tessellation_and_pattern_designer.hpp"

namespace {

testing::TestOutcome run_tessellation_and_pattern_designer() {
    tessellation_and_pattern_designer::init();
    auto s = tessellation_and_pattern_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tessellation_and_pattern_designer::shutdown();
    return testing::ok();
}

const int _reg_tessellation_and_pattern_designer = testing::register_test(
    "tessellation_and_pattern_designer",
    "1878_tessellation_and_pattern_designer: stub status check",
    &run_tessellation_and_pattern_designer);

}
