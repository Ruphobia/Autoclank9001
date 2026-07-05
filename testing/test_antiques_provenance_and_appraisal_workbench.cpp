// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1893_antiques_provenance_and_appraisal_workbench/antiques_provenance_and_appraisal_workbench.hpp"

namespace {

testing::TestOutcome run_antiques_provenance_and_appraisal_workbench() {
    antiques_provenance_and_appraisal_workbench::init();
    auto s = antiques_provenance_and_appraisal_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    antiques_provenance_and_appraisal_workbench::shutdown();
    return testing::ok();
}

const int _reg_antiques_provenance_and_appraisal_workbench = testing::register_test(
    "antiques_provenance_and_appraisal_workbench",
    "1893_antiques_provenance_and_appraisal_workbench: stub status check",
    &run_antiques_provenance_and_appraisal_workbench);

}
