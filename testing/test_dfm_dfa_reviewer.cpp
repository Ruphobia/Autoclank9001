// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/950_dfm_dfa_reviewer/dfm_dfa_reviewer.hpp"

namespace {

testing::TestOutcome run_dfm_dfa_reviewer() {
    dfm_dfa_reviewer::init();
    auto s = dfm_dfa_reviewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dfm_dfa_reviewer::shutdown();
    return testing::ok();
}

const int _reg_dfm_dfa_reviewer = testing::register_test(
    "dfm_dfa_reviewer",
    "950_dfm_dfa_reviewer: stub status check",
    &run_dfm_dfa_reviewer);

}
