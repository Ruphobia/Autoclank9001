// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/574_patent_drafter/patent_drafter.hpp"

namespace {

testing::TestOutcome run_patent_drafter() {
    patent_drafter::init();
    auto s = patent_drafter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    patent_drafter::shutdown();
    return testing::ok();
}

const int _reg_patent_drafter = testing::register_test(
    "patent_drafter",
    "574_patent_drafter: stub status check",
    &run_patent_drafter);

}
