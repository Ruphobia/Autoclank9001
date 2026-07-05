// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1089_tubular_and_casing_designer/tubular_and_casing_designer.hpp"

namespace {

testing::TestOutcome run_tubular_and_casing_designer() {
    tubular_and_casing_designer::init();
    auto s = tubular_and_casing_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tubular_and_casing_designer::shutdown();
    return testing::ok();
}

const int _reg_tubular_and_casing_designer = testing::register_test(
    "tubular_and_casing_designer",
    "1089_tubular_and_casing_designer: stub status check",
    &run_tubular_and_casing_designer);

}
