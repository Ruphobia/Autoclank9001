// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/428_gantt_timeline/gantt_timeline.hpp"

namespace {

testing::TestOutcome run_gantt_timeline() {
    gantt_timeline::init();
    auto s = gantt_timeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gantt_timeline::shutdown();
    return testing::ok();
}

const int _reg_gantt_timeline = testing::register_test(
    "gantt_timeline",
    "428_gantt_timeline: stub status check",
    &run_gantt_timeline);

}
