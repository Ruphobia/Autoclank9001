// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/636_document_diff/document_diff.hpp"

namespace {

testing::TestOutcome run_document_diff() {
    document_diff::init();
    auto s = document_diff::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    document_diff::shutdown();
    return testing::ok();
}

const int _reg_document_diff = testing::register_test(
    "document_diff",
    "636_document_diff: stub status check",
    &run_document_diff);

}
