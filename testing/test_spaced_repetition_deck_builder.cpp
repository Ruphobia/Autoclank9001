// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/515_spaced_repetition_deck_builder/spaced_repetition_deck_builder.hpp"

namespace {

testing::TestOutcome run_spaced_repetition_deck_builder() {
    spaced_repetition_deck_builder::init();
    auto s = spaced_repetition_deck_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spaced_repetition_deck_builder::shutdown();
    return testing::ok();
}

const int _reg_spaced_repetition_deck_builder = testing::register_test(
    "spaced_repetition_deck_builder",
    "515_spaced_repetition_deck_builder: stub status check",
    &run_spaced_repetition_deck_builder);

}
