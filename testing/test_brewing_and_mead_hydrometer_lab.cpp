// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1811_brewing_and_mead_hydrometer_lab/brewing_and_mead_hydrometer_lab.hpp"

namespace {

testing::TestOutcome run_brewing_and_mead_hydrometer_lab() {
    brewing_and_mead_hydrometer_lab::init();
    auto s = brewing_and_mead_hydrometer_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    brewing_and_mead_hydrometer_lab::shutdown();
    return testing::ok();
}

const int _reg_brewing_and_mead_hydrometer_lab = testing::register_test(
    "brewing_and_mead_hydrometer_lab",
    "1811_brewing_and_mead_hydrometer_lab: stub status check",
    &run_brewing_and_mead_hydrometer_lab);

}
