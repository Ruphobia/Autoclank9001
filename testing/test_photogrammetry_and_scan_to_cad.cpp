#include "test_runner.hpp"
#include "../modules/957_photogrammetry_and_scan_to_cad/photogrammetry_and_scan_to_cad.hpp"

namespace {

testing::TestOutcome run_photogrammetry_and_scan_to_cad() {
    photogrammetry_and_scan_to_cad::init();
    auto s = photogrammetry_and_scan_to_cad::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    photogrammetry_and_scan_to_cad::shutdown();
    return testing::ok();
}

const int _reg_photogrammetry_and_scan_to_cad = testing::register_test(
    "photogrammetry_and_scan_to_cad",
    "957_photogrammetry_and_scan_to_cad: stub status check",
    &run_photogrammetry_and_scan_to_cad);

}
