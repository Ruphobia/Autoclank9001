// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1989_meta_tracker/meta_tracker.hpp"

namespace {

testing::TestOutcome run_meta_tracker() {
    meta_tracker::init();
    auto s = meta_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    meta_tracker::shutdown();
    return testing::ok();
}

const int _reg_meta_tracker = testing::register_test(
    "meta_tracker",
    "1989_meta_tracker: stub status check",
    &run_meta_tracker);

}
