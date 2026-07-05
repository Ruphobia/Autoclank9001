// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/400_floppy_flux_workbench/floppy_flux_workbench.hpp"

namespace {

testing::TestOutcome run_floppy_flux_workbench() {
    floppy_flux_workbench::init();
    auto s = floppy_flux_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    floppy_flux_workbench::shutdown();
    return testing::ok();
}

const int _reg_floppy_flux_workbench = testing::register_test(
    "floppy_flux_workbench",
    "400_floppy_flux_workbench: stub status check",
    &run_floppy_flux_workbench);

}
