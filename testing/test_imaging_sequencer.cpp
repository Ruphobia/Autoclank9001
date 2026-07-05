// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1482_imaging_sequencer/imaging_sequencer.hpp"

namespace {

testing::TestOutcome run_imaging_sequencer() {
    imaging_sequencer::init();
    auto s = imaging_sequencer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    imaging_sequencer::shutdown();
    return testing::ok();
}

const int _reg_imaging_sequencer = testing::register_test(
    "imaging_sequencer",
    "1482_imaging_sequencer: stub status check",
    &run_imaging_sequencer);

}
