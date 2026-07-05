#include "test_runner.hpp"
#include "../modules/718_translation_drafting_assistant/translation_drafting_assistant.hpp"

namespace {

testing::TestOutcome run_translation_drafting_assistant() {
    translation_drafting_assistant::init();
    auto s = translation_drafting_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    translation_drafting_assistant::shutdown();
    return testing::ok();
}

const int _reg_translation_drafting_assistant = testing::register_test(
    "translation_drafting_assistant",
    "718_translation_drafting_assistant: stub status check",
    &run_translation_drafting_assistant);

}
