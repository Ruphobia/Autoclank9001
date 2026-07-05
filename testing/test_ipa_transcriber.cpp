#include "test_runner.hpp"
#include "../modules/785_ipa_transcriber/ipa_transcriber.hpp"

namespace {

testing::TestOutcome run_ipa_transcriber() {
    ipa_transcriber::init();
    auto s = ipa_transcriber::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ipa_transcriber::shutdown();
    return testing::ok();
}

const int _reg_ipa_transcriber = testing::register_test(
    "ipa_transcriber",
    "785_ipa_transcriber: stub status check",
    &run_ipa_transcriber);

}
