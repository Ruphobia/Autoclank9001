#include "test_runner.hpp"
#include "../modules/911_quantum_mechanics_sandbox/quantum_mechanics_sandbox.hpp"

namespace {

testing::TestOutcome run_quantum_mechanics_sandbox() {
    quantum_mechanics_sandbox::init();
    auto s = quantum_mechanics_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quantum_mechanics_sandbox::shutdown();
    return testing::ok();
}

const int _reg_quantum_mechanics_sandbox = testing::register_test(
    "quantum_mechanics_sandbox",
    "911_quantum_mechanics_sandbox: stub status check",
    &run_quantum_mechanics_sandbox);

}
