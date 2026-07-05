// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/179_clock_tree_configurator/clock_tree_configurator.hpp"

namespace {

testing::TestOutcome run_clock_tree_configurator() {
    clock_tree_configurator::init();
    auto s = clock_tree_configurator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    clock_tree_configurator::shutdown();
    return testing::ok();
}

const int _reg_clock_tree_configurator = testing::register_test(
    "clock_tree_configurator",
    "179_clock_tree_configurator: stub status check",
    &run_clock_tree_configurator);

}
