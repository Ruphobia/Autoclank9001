// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1384_glycan_and_post_translational_analyzer/glycan_and_post_translational_analyzer.hpp"

namespace {

testing::TestOutcome run_glycan_and_post_translational_analyzer() {
    glycan_and_post_translational_analyzer::init();
    auto s = glycan_and_post_translational_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    glycan_and_post_translational_analyzer::shutdown();
    return testing::ok();
}

const int _reg_glycan_and_post_translational_analyzer = testing::register_test(
    "glycan_and_post_translational_analyzer",
    "1384_glycan_and_post_translational_analyzer: stub status check",
    &run_glycan_and_post_translational_analyzer);

}
