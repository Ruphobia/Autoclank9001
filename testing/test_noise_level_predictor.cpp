// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1009_noise_level_predictor/noise_level_predictor.hpp"

namespace {

testing::TestOutcome run_noise_level_predictor() {
    noise_level_predictor::init();
    auto s = noise_level_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    noise_level_predictor::shutdown();
    return testing::ok();
}

const int _reg_noise_level_predictor = testing::register_test(
    "noise_level_predictor",
    "1009_noise_level_predictor: stub status check",
    &run_noise_level_predictor);

}
