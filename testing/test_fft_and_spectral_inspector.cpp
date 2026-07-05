// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1727_fft_and_spectral_inspector/fft_and_spectral_inspector.hpp"

namespace {

testing::TestOutcome run_fft_and_spectral_inspector() {
    fft_and_spectral_inspector::init();
    auto s = fft_and_spectral_inspector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fft_and_spectral_inspector::shutdown();
    return testing::ok();
}

const int _reg_fft_and_spectral_inspector = testing::register_test(
    "fft_and_spectral_inspector",
    "1727_fft_and_spectral_inspector: stub status check",
    &run_fft_and_spectral_inspector);

}
