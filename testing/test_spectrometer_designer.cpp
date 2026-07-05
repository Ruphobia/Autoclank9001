// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/886_spectrometer_designer/spectrometer_designer.hpp"

namespace {

testing::TestOutcome run_spectrometer_designer() {
    spectrometer_designer::init();
    auto s = spectrometer_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spectrometer_designer::shutdown();
    return testing::ok();
}

const int _reg_spectrometer_designer = testing::register_test(
    "spectrometer_designer",
    "886_spectrometer_designer: stub status check",
    &run_spectrometer_designer);

}
