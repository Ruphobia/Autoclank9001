#include "test_runner.hpp"
#include "../modules/850_waveform_viewer/waveform_viewer.hpp"

namespace {

testing::TestOutcome run_waveform_viewer() {
    waveform_viewer::init();
    auto s = waveform_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    waveform_viewer::shutdown();
    return testing::ok();
}

const int _reg_waveform_viewer = testing::register_test(
    "waveform_viewer",
    "850_waveform_viewer: stub status check",
    &run_waveform_viewer);

}
