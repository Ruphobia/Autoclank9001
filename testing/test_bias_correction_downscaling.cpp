// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1472_bias_correction_downscaling/bias_correction_downscaling.hpp"

namespace {

testing::TestOutcome run_bias_correction_downscaling() {
    bias_correction_downscaling::init();
    auto s = bias_correction_downscaling::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bias_correction_downscaling::shutdown();
    return testing::ok();
}

const int _reg_bias_correction_downscaling = testing::register_test(
    "bias_correction_downscaling",
    "1472_bias_correction_downscaling: stub status check",
    &run_bias_correction_downscaling);

}
