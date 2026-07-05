// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1677_lut_designer/lut_designer.hpp"

namespace {

testing::TestOutcome run_lut_designer() {
    lut_designer::init();
    auto s = lut_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lut_designer::shutdown();
    return testing::ok();
}

const int _reg_lut_designer = testing::register_test(
    "lut_designer",
    "1677_lut_designer: stub status check",
    &run_lut_designer);

}
