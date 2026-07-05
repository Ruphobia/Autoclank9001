#include "test_runner.hpp"
#include "../modules/633_document_summarizer/document_summarizer.hpp"

namespace {

testing::TestOutcome run_document_summarizer() {
    document_summarizer::init();
    auto s = document_summarizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    document_summarizer::shutdown();
    return testing::ok();
}

const int _reg_document_summarizer = testing::register_test(
    "document_summarizer",
    "633_document_summarizer: stub status check",
    &run_document_summarizer);

}
