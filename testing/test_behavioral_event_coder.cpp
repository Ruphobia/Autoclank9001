// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1424_behavioral_event_coder/behavioral_event_coder.hpp"

namespace {

testing::TestOutcome run_behavioral_event_coder() {
    behavioral_event_coder::init();
    auto s = behavioral_event_coder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    behavioral_event_coder::shutdown();
    return testing::ok();
}

const int _reg_behavioral_event_coder = testing::register_test(
    "behavioral_event_coder",
    "1424_behavioral_event_coder: stub status check",
    &run_behavioral_event_coder);

}
