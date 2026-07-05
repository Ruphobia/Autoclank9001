#include "test_runner.hpp"
#include "../modules/1724_multitrack_audio_editor/multitrack_audio_editor.hpp"

namespace {

testing::TestOutcome run_multitrack_audio_editor() {
    multitrack_audio_editor::init();
    auto s = multitrack_audio_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multitrack_audio_editor::shutdown();
    return testing::ok();
}

const int _reg_multitrack_audio_editor = testing::register_test(
    "multitrack_audio_editor",
    "1724_multitrack_audio_editor: stub status check",
    &run_multitrack_audio_editor);

}
