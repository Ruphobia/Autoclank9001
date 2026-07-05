// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1812_vinegar_and_acetification_monitor/vinegar_and_acetification_monitor.hpp"

namespace {

testing::TestOutcome run_vinegar_and_acetification_monitor() {
    vinegar_and_acetification_monitor::init();
    auto s = vinegar_and_acetification_monitor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vinegar_and_acetification_monitor::shutdown();
    return testing::ok();
}

const int _reg_vinegar_and_acetification_monitor = testing::register_test(
    "vinegar_and_acetification_monitor",
    "1812_vinegar_and_acetification_monitor: stub status check",
    &run_vinegar_and_acetification_monitor);

}
