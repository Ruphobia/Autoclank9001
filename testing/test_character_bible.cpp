// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/497_character_bible/character_bible.hpp"

namespace {

testing::TestOutcome run_character_bible() {
    character_bible::init();
    auto s = character_bible::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    character_bible::shutdown();
    return testing::ok();
}

const int _reg_character_bible = testing::register_test(
    "character_bible",
    "497_character_bible: stub status check",
    &run_character_bible);

}
