// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/127_ssr_ssg_isr_strategist/ssr_ssg_isr_strategist.hpp"

namespace {

testing::TestOutcome run_ssr_ssg_isr_strategist() {
    ssr_ssg_isr_strategist::init();
    auto s = ssr_ssg_isr_strategist::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ssr_ssg_isr_strategist::shutdown();
    return testing::ok();
}

const int _reg_ssr_ssg_isr_strategist = testing::register_test(
    "ssr_ssg_isr_strategist",
    "127_ssr_ssg_isr_strategist: stub status check",
    &run_ssr_ssg_isr_strategist);

}
