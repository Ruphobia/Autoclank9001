#include "test_runner.hpp"
#include "../modules/1058_pipe_friction_calculator/pipe_friction_calculator.hpp"

namespace {

testing::TestOutcome run_pipe_friction_calculator() {
    pipe_friction_calculator::init();
    auto s = pipe_friction_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pipe_friction_calculator::shutdown();
    return testing::ok();
}

const int _reg_pipe_friction_calculator = testing::register_test(
    "pipe_friction_calculator",
    "1058_pipe_friction_calculator: stub status check",
    &run_pipe_friction_calculator);

}
