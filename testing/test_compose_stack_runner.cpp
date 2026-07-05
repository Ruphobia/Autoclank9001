#include "test_runner.hpp"
#include "../modules/267_compose_stack_runner/compose_stack_runner.hpp"

namespace {

testing::TestOutcome run_compose_stack_runner() {
    compose_stack_runner::init();
    auto s = compose_stack_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    compose_stack_runner::shutdown();
    return testing::ok();
}

const int _reg_compose_stack_runner = testing::register_test(
    "compose_stack_runner",
    "267_compose_stack_runner: stub status check",
    &run_compose_stack_runner);

}
