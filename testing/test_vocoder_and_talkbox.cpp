#include "test_runner.hpp"
#include "../modules/1753_vocoder_and_talkbox/vocoder_and_talkbox.hpp"

namespace {

testing::TestOutcome run_vocoder_and_talkbox() {
    vocoder_and_talkbox::init();
    auto s = vocoder_and_talkbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vocoder_and_talkbox::shutdown();
    return testing::ok();
}

const int _reg_vocoder_and_talkbox = testing::register_test(
    "vocoder_and_talkbox",
    "1753_vocoder_and_talkbox: stub status check",
    &run_vocoder_and_talkbox);

}
