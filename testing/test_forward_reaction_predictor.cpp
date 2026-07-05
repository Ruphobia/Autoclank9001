// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1301_forward_reaction_predictor/forward_reaction_predictor.hpp"

namespace {

testing::TestOutcome run_forward_reaction_predictor() {
    forward_reaction_predictor::init();
    auto s = forward_reaction_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    forward_reaction_predictor::shutdown();
    return testing::ok();
}

const int _reg_forward_reaction_predictor = testing::register_test(
    "forward_reaction_predictor",
    "1301_forward_reaction_predictor: stub status check",
    &run_forward_reaction_predictor);

}
