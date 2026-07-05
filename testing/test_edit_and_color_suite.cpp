// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1703_edit_and_color_suite/edit_and_color_suite.hpp"

namespace {

testing::TestOutcome run_edit_and_color_suite() {
    edit_and_color_suite::init();
    auto s = edit_and_color_suite::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    edit_and_color_suite::shutdown();
    return testing::ok();
}

const int _reg_edit_and_color_suite = testing::register_test(
    "edit_and_color_suite",
    "1703_edit_and_color_suite: stub status check",
    &run_edit_and_color_suite);

}
