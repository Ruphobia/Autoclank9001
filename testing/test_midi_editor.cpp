#include "test_runner.hpp"
#include "../modules/1739_midi_editor/midi_editor.hpp"

namespace {

testing::TestOutcome run_midi_editor() {
    midi_editor::init();
    auto s = midi_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    midi_editor::shutdown();
    return testing::ok();
}

const int _reg_midi_editor = testing::register_test(
    "midi_editor",
    "1739_midi_editor: stub status check",
    &run_midi_editor);

}
