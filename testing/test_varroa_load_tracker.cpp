// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2005_varroa_load_tracker/varroa_load_tracker.hpp"

namespace {

testing::TestOutcome run_varroa_load_tracker() {
    varroa_load_tracker::init();
    auto s = varroa_load_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    varroa_load_tracker::shutdown();
    return testing::ok();
}

const int _reg_varroa_load_tracker = testing::register_test(
    "varroa_load_tracker",
    "2005_varroa_load_tracker: stub status check",
    &run_varroa_load_tracker);

}
