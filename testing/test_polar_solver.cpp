#include "test_runner.hpp"
#include "../modules/1150_polar_solver/polar_solver.hpp"

namespace {

testing::TestOutcome run_polar_solver() {
    polar_solver::init();
    auto s = polar_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polar_solver::shutdown();
    return testing::ok();
}

const int _reg_polar_solver = testing::register_test(
    "polar_solver",
    "1150_polar_solver: stub status check",
    &run_polar_solver);

}
