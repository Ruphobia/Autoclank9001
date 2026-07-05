#include "test_runner.hpp"
#include "../modules/762_bayesian_belief_sandbox/bayesian_belief_sandbox.hpp"

namespace {

testing::TestOutcome run_bayesian_belief_sandbox() {
    bayesian_belief_sandbox::init();
    auto s = bayesian_belief_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bayesian_belief_sandbox::shutdown();
    return testing::ok();
}

const int _reg_bayesian_belief_sandbox = testing::register_test(
    "bayesian_belief_sandbox",
    "762_bayesian_belief_sandbox: stub status check",
    &run_bayesian_belief_sandbox);

}
