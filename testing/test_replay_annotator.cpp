// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1979_replay_annotator/replay_annotator.hpp"

namespace {

testing::TestOutcome run_replay_annotator() {
    replay_annotator::init();
    auto s = replay_annotator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    replay_annotator::shutdown();
    return testing::ok();
}

const int _reg_replay_annotator = testing::register_test(
    "replay_annotator",
    "1979_replay_annotator: stub status check",
    &run_replay_annotator);

}
