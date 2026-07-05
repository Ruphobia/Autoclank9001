// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/435_document_diff_and_merge/document_diff_and_merge.hpp"

namespace {

testing::TestOutcome run_document_diff_and_merge() {
    document_diff_and_merge::init();
    auto s = document_diff_and_merge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    document_diff_and_merge::shutdown();
    return testing::ok();
}

const int _reg_document_diff_and_merge = testing::register_test(
    "document_diff_and_merge",
    "435_document_diff_and_merge: stub status check",
    &run_document_diff_and_merge);

}
