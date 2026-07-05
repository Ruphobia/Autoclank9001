// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1430_bayesian_analysis_workbench/bayesian_analysis_workbench.hpp"

namespace {

testing::TestOutcome run_bayesian_analysis_workbench() {
    bayesian_analysis_workbench::init();
    auto s = bayesian_analysis_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bayesian_analysis_workbench::shutdown();
    return testing::ok();
}

const int _reg_bayesian_analysis_workbench = testing::register_test(
    "bayesian_analysis_workbench",
    "1430_bayesian_analysis_workbench: stub status check",
    &run_bayesian_analysis_workbench);

}
