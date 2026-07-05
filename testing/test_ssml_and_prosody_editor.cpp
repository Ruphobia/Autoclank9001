#include "test_runner.hpp"
#include "../modules/835_ssml_and_prosody_editor/ssml_and_prosody_editor.hpp"

namespace {

testing::TestOutcome run_ssml_and_prosody_editor() {
    ssml_and_prosody_editor::init();
    auto s = ssml_and_prosody_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ssml_and_prosody_editor::shutdown();
    return testing::ok();
}

const int _reg_ssml_and_prosody_editor = testing::register_test(
    "ssml_and_prosody_editor",
    "835_ssml_and_prosody_editor: stub status check",
    &run_ssml_and_prosody_editor);

}
