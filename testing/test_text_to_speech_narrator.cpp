#include "test_runner.hpp"
#include "../modules/638_text_to_speech_narrator/text_to_speech_narrator.hpp"

namespace {

testing::TestOutcome run_text_to_speech_narrator() {
    text_to_speech_narrator::init();
    auto s = text_to_speech_narrator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    text_to_speech_narrator::shutdown();
    return testing::ok();
}

const int _reg_text_to_speech_narrator = testing::register_test(
    "text_to_speech_narrator",
    "638_text_to_speech_narrator: stub status check",
    &run_text_to_speech_narrator);

}
