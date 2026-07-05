// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/621_layout_analyzer/layout_analyzer.hpp"

namespace {

testing::TestOutcome run_layout_analyzer() {
    layout_analyzer::init();
    auto s = layout_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    layout_analyzer::shutdown();
    return testing::ok();
}

const int _reg_layout_analyzer = testing::register_test(
    "layout_analyzer",
    "621_layout_analyzer: stub status check",
    &run_layout_analyzer);

}
