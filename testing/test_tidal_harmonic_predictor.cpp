// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2094_tidal_harmonic_predictor/tidal_harmonic_predictor.hpp"

namespace {

testing::TestOutcome run_tidal_harmonic_predictor() {
    tidal_harmonic_predictor::init();
    auto s = tidal_harmonic_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tidal_harmonic_predictor::shutdown();
    return testing::ok();
}

const int _reg_tidal_harmonic_predictor = testing::register_test(
    "tidal_harmonic_predictor",
    "2094_tidal_harmonic_predictor: stub status check",
    &run_tidal_harmonic_predictor);

}
