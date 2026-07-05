#include "test_runner.hpp"
#include "../modules/659_bayesian_sandbox/bayesian_sandbox.hpp"

namespace {

testing::TestOutcome run_bayesian_sandbox() {
    bayesian_sandbox::init();
    auto s = bayesian_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bayesian_sandbox::shutdown();
    return testing::ok();
}

const int _reg_bayesian_sandbox = testing::register_test(
    "bayesian_sandbox",
    "659_bayesian_sandbox: stub status check",
    &run_bayesian_sandbox);

}
