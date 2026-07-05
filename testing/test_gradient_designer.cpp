// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1863_gradient_designer/gradient_designer.hpp"

namespace {

testing::TestOutcome run_gradient_designer() {
    gradient_designer::init();
    auto s = gradient_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gradient_designer::shutdown();
    return testing::ok();
}

const int _reg_gradient_designer = testing::register_test(
    "gradient_designer",
    "1863_gradient_designer: stub status check",
    &run_gradient_designer);

}
