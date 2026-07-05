// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1298_reaction_balancer/reaction_balancer.hpp"

namespace {

testing::TestOutcome run_reaction_balancer() {
    reaction_balancer::init();
    auto s = reaction_balancer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reaction_balancer::shutdown();
    return testing::ok();
}

const int _reg_reaction_balancer = testing::register_test(
    "reaction_balancer",
    "1298_reaction_balancer: stub status check",
    &run_reaction_balancer);

}
