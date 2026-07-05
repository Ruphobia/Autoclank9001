// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/855_emc_pre_compliance/emc_pre_compliance.hpp"

namespace {

testing::TestOutcome run_emc_pre_compliance() {
    emc_pre_compliance::init();
    auto s = emc_pre_compliance::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    emc_pre_compliance::shutdown();
    return testing::ok();
}

const int _reg_emc_pre_compliance = testing::register_test(
    "emc_pre_compliance",
    "855_emc_pre_compliance: stub status check",
    &run_emc_pre_compliance);

}
