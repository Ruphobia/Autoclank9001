// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/256_resource_monitor/resource_monitor.hpp"

namespace {

testing::TestOutcome run_resource_monitor() {
    resource_monitor::init();
    auto s = resource_monitor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    resource_monitor::shutdown();
    return testing::ok();
}

const int _reg_resource_monitor = testing::register_test(
    "resource_monitor",
    "256_resource_monitor: stub status check",
    &run_resource_monitor);

}
