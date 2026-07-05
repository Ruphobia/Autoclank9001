// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1153_transition_predictor/transition_predictor.hpp"

namespace {

testing::TestOutcome run_transition_predictor() {
    transition_predictor::init();
    auto s = transition_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    transition_predictor::shutdown();
    return testing::ok();
}

const int _reg_transition_predictor = testing::register_test(
    "transition_predictor",
    "1153_transition_predictor: stub status check",
    &run_transition_predictor);

}
