#include "test_runner.hpp"
#include "../modules/412_word_processor/word_processor.hpp"

namespace {

testing::TestOutcome run_word_processor() {
    word_processor::init();
    auto s = word_processor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    word_processor::shutdown();
    return testing::ok();
}

const int _reg_word_processor = testing::register_test(
    "word_processor",
    "412_word_processor: stub status check",
    &run_word_processor);

}
