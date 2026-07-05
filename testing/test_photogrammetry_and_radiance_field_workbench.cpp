// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1265_photogrammetry_and_radiance_field_workbench/photogrammetry_and_radiance_field_workbench.hpp"

namespace {

testing::TestOutcome run_photogrammetry_and_radiance_field_workbench() {
    photogrammetry_and_radiance_field_workbench::init();
    auto s = photogrammetry_and_radiance_field_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    photogrammetry_and_radiance_field_workbench::shutdown();
    return testing::ok();
}

const int _reg_photogrammetry_and_radiance_field_workbench = testing::register_test(
    "photogrammetry_and_radiance_field_workbench",
    "1265_photogrammetry_and_radiance_field_workbench: stub status check",
    &run_photogrammetry_and_radiance_field_workbench);

}
