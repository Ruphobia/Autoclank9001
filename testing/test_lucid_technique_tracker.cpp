// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2155_lucid_technique_tracker/lucid_technique_tracker.hpp"

namespace {

testing::TestOutcome run_lucid_technique_tracker() {
    lucid_technique_tracker::init();
    auto s = lucid_technique_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lucid_technique_tracker::shutdown();
    return testing::ok();
}

const int _reg_lucid_technique_tracker = testing::register_test(
    "lucid_technique_tracker",
    "2155_lucid_technique_tracker: stub status check",
    &run_lucid_technique_tracker);

}
