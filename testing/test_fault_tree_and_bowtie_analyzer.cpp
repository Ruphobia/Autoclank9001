// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/466_fault_tree_and_bowtie_analyzer/fault_tree_and_bowtie_analyzer.hpp"

namespace {

testing::TestOutcome run_fault_tree_and_bowtie_analyzer() {
    fault_tree_and_bowtie_analyzer::init();
    auto s = fault_tree_and_bowtie_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fault_tree_and_bowtie_analyzer::shutdown();
    return testing::ok();
}

const int _reg_fault_tree_and_bowtie_analyzer = testing::register_test(
    "fault_tree_and_bowtie_analyzer",
    "466_fault_tree_and_bowtie_analyzer: stub status check",
    &run_fault_tree_and_bowtie_analyzer);

}
