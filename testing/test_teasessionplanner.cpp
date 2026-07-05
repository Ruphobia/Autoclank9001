// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1824_teasessionplanner/teasessionplanner.hpp"

namespace {

testing::TestOutcome run_teasessionplanner() {
    teasessionplanner::init();
    auto s = teasessionplanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    teasessionplanner::shutdown();
    return testing::ok();
}

const int _reg_teasessionplanner = testing::register_test(
    "teasessionplanner",
    "1824_teasessionplanner: stub status check",
    &run_teasessionplanner);

}
