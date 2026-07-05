// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1474_wind_rose_climatology_builder/wind_rose_climatology_builder.hpp"

namespace {

testing::TestOutcome run_wind_rose_climatology_builder() {
    wind_rose_climatology_builder::init();
    auto s = wind_rose_climatology_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    wind_rose_climatology_builder::shutdown();
    return testing::ok();
}

const int _reg_wind_rose_climatology_builder = testing::register_test(
    "wind_rose_climatology_builder",
    "1474_wind_rose_climatology_builder: stub status check",
    &run_wind_rose_climatology_builder);

}
