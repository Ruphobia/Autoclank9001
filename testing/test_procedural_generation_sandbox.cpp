#include "test_runner.hpp"
#include "../modules/301_procedural_generation_sandbox/procedural_generation_sandbox.hpp"

namespace {

testing::TestOutcome run_procedural_generation_sandbox() {
    procedural_generation_sandbox::init();
    auto s = procedural_generation_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    procedural_generation_sandbox::shutdown();
    return testing::ok();
}

const int _reg_procedural_generation_sandbox = testing::register_test(
    "procedural_generation_sandbox",
    "301_procedural_generation_sandbox: stub status check",
    &run_procedural_generation_sandbox);

}
