// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1014_octave_band_spectrum_tool/octave_band_spectrum_tool.hpp"

namespace {

testing::TestOutcome run_octave_band_spectrum_tool() {
    octave_band_spectrum_tool::init();
    auto s = octave_band_spectrum_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    octave_band_spectrum_tool::shutdown();
    return testing::ok();
}

const int _reg_octave_band_spectrum_tool = testing::register_test(
    "octave_band_spectrum_tool",
    "1014_octave_band_spectrum_tool: stub status check",
    &run_octave_band_spectrum_tool);

}
