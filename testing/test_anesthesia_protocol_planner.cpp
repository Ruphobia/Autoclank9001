// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1503_anesthesia_protocol_planner/anesthesia_protocol_planner.hpp"

namespace {

testing::TestOutcome run_anesthesia_protocol_planner() {
    anesthesia_protocol_planner::init();
    auto s = anesthesia_protocol_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anesthesia_protocol_planner::shutdown();
    return testing::ok();
}

const int _reg_anesthesia_protocol_planner = testing::register_test(
    "anesthesia_protocol_planner",
    "1503_anesthesia_protocol_planner: stub status check",
    &run_anesthesia_protocol_planner);

}
