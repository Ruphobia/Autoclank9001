// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1729_convolution_and_impulse_response_toolkit/convolution_and_impulse_response_toolkit.hpp"

namespace {

testing::TestOutcome run_convolution_and_impulse_response_toolkit() {
    convolution_and_impulse_response_toolkit::init();
    auto s = convolution_and_impulse_response_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    convolution_and_impulse_response_toolkit::shutdown();
    return testing::ok();
}

const int _reg_convolution_and_impulse_response_toolkit = testing::register_test(
    "convolution_and_impulse_response_toolkit",
    "1729_convolution_and_impulse_response_toolkit: stub status check",
    &run_convolution_and_impulse_response_toolkit);

}
