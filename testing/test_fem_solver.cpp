#include "test_runner.hpp"
#include "../modules/903_fem_solver/fem_solver.hpp"

namespace {

testing::TestOutcome run_fem_solver() {
    fem_solver::init();
    auto s = fem_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fem_solver::shutdown();
    return testing::ok();
}

const int _reg_fem_solver = testing::register_test(
    "fem_solver",
    "903_fem_solver: stub status check",
    &run_fem_solver);

}
