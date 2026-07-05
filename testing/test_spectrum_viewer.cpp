#include "test_runner.hpp"
#include "../modules/1305_spectrum_viewer/spectrum_viewer.hpp"

namespace {

testing::TestOutcome run_spectrum_viewer() {
    spectrum_viewer::init();
    auto s = spectrum_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spectrum_viewer::shutdown();
    return testing::ok();
}

const int _reg_spectrum_viewer = testing::register_test(
    "spectrum_viewer",
    "1305_spectrum_viewer: stub status check",
    &run_spectrum_viewer);

}
