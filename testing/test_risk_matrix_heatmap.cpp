// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/468_risk_matrix_heatmap/risk_matrix_heatmap.hpp"

namespace {

testing::TestOutcome run_risk_matrix_heatmap() {
    risk_matrix_heatmap::init();
    auto s = risk_matrix_heatmap::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    risk_matrix_heatmap::shutdown();
    return testing::ok();
}

const int _reg_risk_matrix_heatmap = testing::register_test(
    "risk_matrix_heatmap",
    "468_risk_matrix_heatmap: stub status check",
    &run_risk_matrix_heatmap);

}
