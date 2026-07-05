// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1446_causal_inference_kit/causal_inference_kit.hpp"

namespace {

testing::TestOutcome run_causal_inference_kit() {
    causal_inference_kit::init();
    auto s = causal_inference_kit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    causal_inference_kit::shutdown();
    return testing::ok();
}

const int _reg_causal_inference_kit = testing::register_test(
    "causal_inference_kit",
    "1446_causal_inference_kit: stub status check",
    &run_causal_inference_kit);

}
