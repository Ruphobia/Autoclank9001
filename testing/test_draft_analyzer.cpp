// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1987_draft_analyzer/draft_analyzer.hpp"

namespace {

testing::TestOutcome run_draft_analyzer() {
    draft_analyzer::init();
    auto s = draft_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    draft_analyzer::shutdown();
    return testing::ok();
}

const int _reg_draft_analyzer = testing::register_test(
    "draft_analyzer",
    "1987_draft_analyzer: stub status check",
    &run_draft_analyzer);

}
