#include "test_runner.hpp"
#include "../modules/1630_tagger_and_captioner/tagger_and_captioner.hpp"

namespace {

testing::TestOutcome run_tagger_and_captioner() {
    tagger_and_captioner::init();
    auto s = tagger_and_captioner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tagger_and_captioner::shutdown();
    return testing::ok();
}

const int _reg_tagger_and_captioner = testing::register_test(
    "tagger_and_captioner",
    "1630_tagger_and_captioner: stub status check",
    &run_tagger_and_captioner);

}
