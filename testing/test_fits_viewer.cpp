#include "test_runner.hpp"
#include "../modules/1477_fits_viewer/fits_viewer.hpp"

namespace {

testing::TestOutcome run_fits_viewer() {
    fits_viewer::init();
    auto s = fits_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fits_viewer::shutdown();
    return testing::ok();
}

const int _reg_fits_viewer = testing::register_test(
    "fits_viewer",
    "1477_fits_viewer: stub status check",
    &run_fits_viewer);

}
