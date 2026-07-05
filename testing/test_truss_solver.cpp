#include "test_runner.hpp"
#include "../modules/1033_truss_solver/truss_solver.hpp"

namespace {

testing::TestOutcome run_truss_solver() {
    truss_solver::init();
    auto s = truss_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    truss_solver::shutdown();
    return testing::ok();
}

const int _reg_truss_solver = testing::register_test(
    "truss_solver",
    "1033_truss_solver: stub status check",
    &run_truss_solver);

}
