// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1008_msds_and_hazard_lookup/msds_and_hazard_lookup.hpp"

namespace {

testing::TestOutcome run_msds_and_hazard_lookup() {
    msds_and_hazard_lookup::init();
    auto s = msds_and_hazard_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    msds_and_hazard_lookup::shutdown();
    return testing::ok();
}

const int _reg_msds_and_hazard_lookup = testing::register_test(
    "msds_and_hazard_lookup",
    "1008_msds_and_hazard_lookup: stub status check",
    &run_msds_and_hazard_lookup);

}
