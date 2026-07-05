// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/881_diffraction_grating_designer/diffraction_grating_designer.hpp"

namespace {

testing::TestOutcome run_diffraction_grating_designer() {
    diffraction_grating_designer::init();
    auto s = diffraction_grating_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    diffraction_grating_designer::shutdown();
    return testing::ok();
}

const int _reg_diffraction_grating_designer = testing::register_test(
    "diffraction_grating_designer",
    "881_diffraction_grating_designer: stub status check",
    &run_diffraction_grating_designer);

}
