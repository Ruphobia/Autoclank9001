// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1367_bcr_tcr_repertoire_analyzer/bcr_tcr_repertoire_analyzer.hpp"

namespace {

testing::TestOutcome run_bcr_tcr_repertoire_analyzer() {
    bcr_tcr_repertoire_analyzer::init();
    auto s = bcr_tcr_repertoire_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bcr_tcr_repertoire_analyzer::shutdown();
    return testing::ok();
}

const int _reg_bcr_tcr_repertoire_analyzer = testing::register_test(
    "bcr_tcr_repertoire_analyzer",
    "1367_bcr_tcr_repertoire_analyzer: stub status check",
    &run_bcr_tcr_repertoire_analyzer);

}
