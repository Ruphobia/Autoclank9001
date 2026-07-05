// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1450_document_text_analyzer/document_text_analyzer.hpp"

namespace {

testing::TestOutcome run_document_text_analyzer() {
    document_text_analyzer::init();
    auto s = document_text_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    document_text_analyzer::shutdown();
    return testing::ok();
}

const int _reg_document_text_analyzer = testing::register_test(
    "document_text_analyzer",
    "1450_document_text_analyzer: stub status check",
    &run_document_text_analyzer);

}
