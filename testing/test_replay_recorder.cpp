// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/309_replay_recorder/replay_recorder.hpp"

namespace {

testing::TestOutcome run_replay_recorder() {
    replay_recorder::init();
    auto s = replay_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    replay_recorder::shutdown();
    return testing::ok();
}

const int _reg_replay_recorder = testing::register_test(
    "replay_recorder",
    "309_replay_recorder: stub status check",
    &run_replay_recorder);

}
