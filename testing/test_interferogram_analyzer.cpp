// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/879_interferogram_analyzer/interferogram_analyzer.hpp"

namespace {

testing::TestOutcome run_interferogram_analyzer() {
    interferogram_analyzer::init();
    auto s = interferogram_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    interferogram_analyzer::shutdown();
    return testing::ok();
}

const int _reg_interferogram_analyzer = testing::register_test(
    "interferogram_analyzer",
    "879_interferogram_analyzer: stub status check",
    &run_interferogram_analyzer);

}
