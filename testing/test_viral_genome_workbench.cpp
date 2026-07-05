// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1361_viral_genome_workbench/viral_genome_workbench.hpp"

namespace {

testing::TestOutcome run_viral_genome_workbench() {
    viral_genome_workbench::init();
    auto s = viral_genome_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    viral_genome_workbench::shutdown();
    return testing::ok();
}

const int _reg_viral_genome_workbench = testing::register_test(
    "viral_genome_workbench",
    "1361_viral_genome_workbench: stub status check",
    &run_viral_genome_workbench);

}
