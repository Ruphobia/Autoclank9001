// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/453_meeting_notes_and_action_extractor/meeting_notes_and_action_extractor.hpp"

namespace {

testing::TestOutcome run_meeting_notes_and_action_extractor() {
    meeting_notes_and_action_extractor::init();
    auto s = meeting_notes_and_action_extractor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    meeting_notes_and_action_extractor::shutdown();
    return testing::ok();
}

const int _reg_meeting_notes_and_action_extractor = testing::register_test(
    "meeting_notes_and_action_extractor",
    "453_meeting_notes_and_action_extractor: stub status check",
    &run_meeting_notes_and_action_extractor);

}
