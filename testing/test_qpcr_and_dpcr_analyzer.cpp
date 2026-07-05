// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1373_qpcr_and_dpcr_analyzer/qpcr_and_dpcr_analyzer.hpp"

namespace {

testing::TestOutcome run_qpcr_and_dpcr_analyzer() {
    qpcr_and_dpcr_analyzer::init();
    auto s = qpcr_and_dpcr_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    qpcr_and_dpcr_analyzer::shutdown();
    return testing::ok();
}

const int _reg_qpcr_and_dpcr_analyzer = testing::register_test(
    "qpcr_and_dpcr_analyzer",
    "1373_qpcr_and_dpcr_analyzer: stub status check",
    &run_qpcr_and_dpcr_analyzer);

}
