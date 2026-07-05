#include "test_runner.hpp"
#include "../modules/897_ode_pde_solver/ode_pde_solver.hpp"

namespace {

testing::TestOutcome run_ode_pde_solver() {
    ode_pde_solver::init();
    auto s = ode_pde_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ode_pde_solver::shutdown();
    return testing::ok();
}

const int _reg_ode_pde_solver = testing::register_test(
    "ode_pde_solver",
    "897_ode_pde_solver: stub status check",
    &run_ode_pde_solver);

}
