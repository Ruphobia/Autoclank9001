// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/884_radiometry_photometry_bench/radiometry_photometry_bench.hpp"

namespace {

testing::TestOutcome run_radiometry_photometry_bench() {
    radiometry_photometry_bench::init();
    auto s = radiometry_photometry_bench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    radiometry_photometry_bench::shutdown();
    return testing::ok();
}

const int _reg_radiometry_photometry_bench = testing::register_test(
    "radiometry_photometry_bench",
    "884_radiometry_photometry_bench: stub status check",
    &run_radiometry_photometry_bench);

}
