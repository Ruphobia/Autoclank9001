// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/531_note_to_deck_converter/note_to_deck_converter.hpp"

namespace {

testing::TestOutcome run_note_to_deck_converter() {
    note_to_deck_converter::init();
    auto s = note_to_deck_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    note_to_deck_converter::shutdown();
    return testing::ok();
}

const int _reg_note_to_deck_converter = testing::register_test(
    "note_to_deck_converter",
    "531_note_to_deck_converter: stub status check",
    &run_note_to_deck_converter);

}
