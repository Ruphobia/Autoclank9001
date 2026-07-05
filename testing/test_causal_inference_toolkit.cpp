// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/662_causal_inference_toolkit/causal_inference_toolkit.hpp"

namespace {

testing::TestOutcome run_causal_inference_toolkit() {
    causal_inference_toolkit::init();
    auto s = causal_inference_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    causal_inference_toolkit::shutdown();
    return testing::ok();
}

const int _reg_causal_inference_toolkit = testing::register_test(
    "causal_inference_toolkit",
    "662_causal_inference_toolkit: stub status check",
    &run_causal_inference_toolkit);

}
