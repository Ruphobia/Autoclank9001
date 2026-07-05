// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1563_occurrence_record_harvester/occurrence_record_harvester.hpp"

namespace {

testing::TestOutcome run_occurrence_record_harvester() {
    occurrence_record_harvester::init();
    auto s = occurrence_record_harvester::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    occurrence_record_harvester::shutdown();
    return testing::ok();
}

const int _reg_occurrence_record_harvester = testing::register_test(
    "occurrence_record_harvester",
    "1563_occurrence_record_harvester: stub status check",
    &run_occurrence_record_harvester);

}
