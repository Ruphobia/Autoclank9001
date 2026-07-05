#include "test_runner.hpp"
#include "../modules/642_ai_summarizer_and_tldr_stack/ai_summarizer_and_tldr_stack.hpp"

namespace {

testing::TestOutcome run_ai_summarizer_and_tldr_stack() {
    ai_summarizer_and_tldr_stack::init();
    auto s = ai_summarizer_and_tldr_stack::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ai_summarizer_and_tldr_stack::shutdown();
    return testing::ok();
}

const int _reg_ai_summarizer_and_tldr_stack = testing::register_test(
    "ai_summarizer_and_tldr_stack",
    "642_ai_summarizer_and_tldr_stack: stub status check",
    &run_ai_summarizer_and_tldr_stack);

}
