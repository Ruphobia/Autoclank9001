#include "test_runner.hpp"
#include "../modules/1479_aperture_photometry/aperture_photometry.hpp"

namespace {

testing::TestOutcome run_aperture_photometry() {
    aperture_photometry::init();
    auto s = aperture_photometry::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    aperture_photometry::shutdown();
    return testing::ok();
}

const int _reg_aperture_photometry = testing::register_test(
    "aperture_photometry",
    "1479_aperture_photometry: stub status check",
    &run_aperture_photometry);

}
