// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/336_noise_model_designer/noise_model_designer.hpp"

namespace {

testing::TestOutcome run_noise_model_designer() {
    noise_model_designer::init();
    auto s = noise_model_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    noise_model_designer::shutdown();
    return testing::ok();
}

const int _reg_noise_model_designer = testing::register_test(
    "noise_model_designer",
    "336_noise_model_designer: stub status check",
    &run_noise_model_designer);

}
