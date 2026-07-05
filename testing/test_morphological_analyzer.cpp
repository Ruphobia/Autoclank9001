// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/756_morphological_analyzer/morphological_analyzer.hpp"

namespace {

testing::TestOutcome run_morphological_analyzer() {
    morphological_analyzer::init();
    auto s = morphological_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    morphological_analyzer::shutdown();
    return testing::ok();
}

const int _reg_morphological_analyzer = testing::register_test(
    "morphological_analyzer",
    "756_morphological_analyzer: stub status check",
    &run_morphological_analyzer);

}
