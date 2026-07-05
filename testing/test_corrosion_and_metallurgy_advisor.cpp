// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1113_corrosion_and_metallurgy_advisor/corrosion_and_metallurgy_advisor.hpp"

namespace {

testing::TestOutcome run_corrosion_and_metallurgy_advisor() {
    corrosion_and_metallurgy_advisor::init();
    auto s = corrosion_and_metallurgy_advisor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    corrosion_and_metallurgy_advisor::shutdown();
    return testing::ok();
}

const int _reg_corrosion_and_metallurgy_advisor = testing::register_test(
    "corrosion_and_metallurgy_advisor",
    "1113_corrosion_and_metallurgy_advisor: stub status check",
    &run_corrosion_and_metallurgy_advisor);

}
