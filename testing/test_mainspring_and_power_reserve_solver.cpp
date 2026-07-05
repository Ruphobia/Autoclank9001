#include "test_runner.hpp"
#include "../modules/1840_mainspring_and_power_reserve_solver/mainspring_and_power_reserve_solver.hpp"

namespace {

testing::TestOutcome run_mainspring_and_power_reserve_solver() {
    mainspring_and_power_reserve_solver::init();
    auto s = mainspring_and_power_reserve_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mainspring_and_power_reserve_solver::shutdown();
    return testing::ok();
}

const int _reg_mainspring_and_power_reserve_solver = testing::register_test(
    "mainspring_and_power_reserve_solver",
    "1840_mainspring_and_power_reserve_solver: stub status check",
    &run_mainspring_and_power_reserve_solver);

}
