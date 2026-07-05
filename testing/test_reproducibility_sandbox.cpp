#include "test_runner.hpp"
#include "../modules/649_reproducibility_sandbox/reproducibility_sandbox.hpp"

namespace {

testing::TestOutcome run_reproducibility_sandbox() {
    reproducibility_sandbox::init();
    auto s = reproducibility_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reproducibility_sandbox::shutdown();
    return testing::ok();
}

const int _reg_reproducibility_sandbox = testing::register_test(
    "reproducibility_sandbox",
    "649_reproducibility_sandbox: stub status check",
    &run_reproducibility_sandbox);

}
