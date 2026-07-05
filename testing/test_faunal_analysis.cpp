// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1530_faunal_analysis/faunal_analysis.hpp"

namespace {

testing::TestOutcome run_faunal_analysis() {
    faunal_analysis::init();
    auto s = faunal_analysis::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    faunal_analysis::shutdown();
    return testing::ok();
}

const int _reg_faunal_analysis = testing::register_test(
    "faunal_analysis",
    "1530_faunal_analysis: stub status check",
    &run_faunal_analysis);

}
