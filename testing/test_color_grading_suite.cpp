// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1676_color_grading_suite/color_grading_suite.hpp"

namespace {

testing::TestOutcome run_color_grading_suite() {
    color_grading_suite::init();
    auto s = color_grading_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    color_grading_suite::shutdown();
    return testing::ok();
}

const int _reg_color_grading_suite = testing::register_test(
    "color_grading_suite",
    "1676_color_grading_suite: stub status check",
    &run_color_grading_suite);

}
