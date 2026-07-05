// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1882_mint_mark_and_variety_tracker/mint_mark_and_variety_tracker.hpp"

namespace {

testing::TestOutcome run_mint_mark_and_variety_tracker() {
    mint_mark_and_variety_tracker::init();
    auto s = mint_mark_and_variety_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mint_mark_and_variety_tracker::shutdown();
    return testing::ok();
}

const int _reg_mint_mark_and_variety_tracker = testing::register_test(
    "mint_mark_and_variety_tracker",
    "1882_mint_mark_and_variety_tracker: stub status check",
    &run_mint_mark_and_variety_tracker);

}
