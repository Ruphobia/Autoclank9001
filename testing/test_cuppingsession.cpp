// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1833_cuppingsession/cuppingsession.hpp"

namespace {

testing::TestOutcome run_cuppingsession() {
    cuppingsession::init();
    auto s = cuppingsession::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cuppingsession::shutdown();
    return testing::ok();
}

const int _reg_cuppingsession = testing::register_test(
    "cuppingsession",
    "1833_cuppingsession: stub status check",
    &run_cuppingsession);

}
