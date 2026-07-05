// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/321_photogrammetry_and_splat_importer/photogrammetry_and_splat_importer.hpp"

namespace {

testing::TestOutcome run_photogrammetry_and_splat_importer() {
    photogrammetry_and_splat_importer::init();
    auto s = photogrammetry_and_splat_importer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    photogrammetry_and_splat_importer::shutdown();
    return testing::ok();
}

const int _reg_photogrammetry_and_splat_importer = testing::register_test(
    "photogrammetry_and_splat_importer",
    "321_photogrammetry_and_splat_importer: stub status check",
    &run_photogrammetry_and_splat_importer);

}
