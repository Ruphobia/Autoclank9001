// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1436_behavioral_game_lab/behavioral_game_lab.hpp"

namespace {

testing::TestOutcome run_behavioral_game_lab() {
    behavioral_game_lab::init();
    auto s = behavioral_game_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    behavioral_game_lab::shutdown();
    return testing::ok();
}

const int _reg_behavioral_game_lab = testing::register_test(
    "behavioral_game_lab",
    "1436_behavioral_game_lab: stub status check",
    &run_behavioral_game_lab);

}
