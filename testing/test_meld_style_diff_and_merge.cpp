// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/114_meld_style_diff_and_merge/meld_style_diff_and_merge.hpp"

namespace {

testing::TestOutcome run_meld_style_diff_and_merge() {
    meld_style_diff_and_merge::init();
    auto s = meld_style_diff_and_merge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    meld_style_diff_and_merge::shutdown();
    return testing::ok();
}

const int _reg_meld_style_diff_and_merge = testing::register_test(
    "meld_style_diff_and_merge",
    "114_meld_style_diff_and_merge: stub status check",
    &run_meld_style_diff_and_merge);

}
