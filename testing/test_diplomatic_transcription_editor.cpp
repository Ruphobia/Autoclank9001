#include "test_runner.hpp"
#include "../modules/687_diplomatic_transcription_editor/diplomatic_transcription_editor.hpp"

namespace {

testing::TestOutcome run_diplomatic_transcription_editor() {
    diplomatic_transcription_editor::init();
    auto s = diplomatic_transcription_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    diplomatic_transcription_editor::shutdown();
    return testing::ok();
}

const int _reg_diplomatic_transcription_editor = testing::register_test(
    "diplomatic_transcription_editor",
    "687_diplomatic_transcription_editor: stub status check",
    &run_diplomatic_transcription_editor);

}
