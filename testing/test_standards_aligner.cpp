// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/521_standards_aligner/standards_aligner.hpp"

namespace {

testing::TestOutcome run_standards_aligner() {
    standards_aligner::init();
    auto s = standards_aligner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    standards_aligner::shutdown();
    return testing::ok();
}

const int _reg_standards_aligner = testing::register_test(
    "standards_aligner",
    "521_standards_aligner: stub status check",
    &run_standards_aligner);

}
