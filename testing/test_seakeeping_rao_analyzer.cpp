// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1075_seakeeping_rao_analyzer/seakeeping_rao_analyzer.hpp"

namespace {

testing::TestOutcome run_seakeeping_rao_analyzer() {
    seakeeping_rao_analyzer::init();
    auto s = seakeeping_rao_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    seakeeping_rao_analyzer::shutdown();
    return testing::ok();
}

const int _reg_seakeeping_rao_analyzer = testing::register_test(
    "seakeeping_rao_analyzer",
    "1075_seakeeping_rao_analyzer: stub status check",
    &run_seakeeping_rao_analyzer);

}
