// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/599_document_classifier/document_classifier.hpp"

namespace {

testing::TestOutcome run_document_classifier() {
    document_classifier::init();
    auto s = document_classifier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    document_classifier::shutdown();
    return testing::ok();
}

const int _reg_document_classifier = testing::register_test(
    "document_classifier",
    "599_document_classifier: stub status check",
    &run_document_classifier);

}
