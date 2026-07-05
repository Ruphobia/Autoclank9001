// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1455_grib2_netcdf_inspector/grib2_netcdf_inspector.hpp"

namespace {

testing::TestOutcome run_grib2_netcdf_inspector() {
    grib2_netcdf_inspector::init();
    auto s = grib2_netcdf_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    grib2_netcdf_inspector::shutdown();
    return testing::ok();
}

const int _reg_grib2_netcdf_inspector = testing::register_test(
    "grib2_netcdf_inspector",
    "1455_grib2_netcdf_inspector: stub status check",
    &run_grib2_netcdf_inspector);

}
