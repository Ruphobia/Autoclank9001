#include "test_runner.hpp"
#include "../modules/101_container_sandbox/container_sandbox.hpp"

namespace {

testing::TestOutcome run_container_sandbox() {
    container_sandbox::init();
    auto s = container_sandbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    container_sandbox::shutdown();
    return testing::ok();
}

const int _reg_container_sandbox = testing::register_test(
    "container_sandbox",
    "101_container_sandbox: stub status check",
    &run_container_sandbox);

}
