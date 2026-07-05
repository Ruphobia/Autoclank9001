// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1304_mass_spectrum_workbench/mass_spectrum_workbench.hpp"

namespace {

testing::TestOutcome run_mass_spectrum_workbench() {
    mass_spectrum_workbench::init();
    auto s = mass_spectrum_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mass_spectrum_workbench::shutdown();
    return testing::ok();
}

const int _reg_mass_spectrum_workbench = testing::register_test(
    "mass_spectrum_workbench",
    "1304_mass_spectrum_workbench: stub status check",
    &run_mass_spectrum_workbench);

}
