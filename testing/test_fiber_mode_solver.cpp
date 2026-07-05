#include "test_runner.hpp"
#include "../modules/877_fiber_mode_solver/fiber_mode_solver.hpp"

namespace {

testing::TestOutcome run_fiber_mode_solver() {
    fiber_mode_solver::init();
    auto s = fiber_mode_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fiber_mode_solver::shutdown();
    return testing::ok();
}

const int _reg_fiber_mode_solver = testing::register_test(
    "fiber_mode_solver",
    "877_fiber_mode_solver: stub status check",
    &run_fiber_mode_solver);

}
