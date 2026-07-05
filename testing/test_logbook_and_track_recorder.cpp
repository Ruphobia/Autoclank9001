// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2107_logbook_and_track_recorder/logbook_and_track_recorder.hpp"

namespace {

testing::TestOutcome run_logbook_and_track_recorder() {
    logbook_and_track_recorder::init();
    auto s = logbook_and_track_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    logbook_and_track_recorder::shutdown();
    return testing::ok();
}

const int _reg_logbook_and_track_recorder = testing::register_test(
    "logbook_and_track_recorder",
    "2107_logbook_and_track_recorder: stub status check",
    &run_logbook_and_track_recorder);

}
