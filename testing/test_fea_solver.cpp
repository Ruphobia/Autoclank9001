#include "test_runner.hpp"
#include "../modules/931_fea_solver/fea_solver.hpp"

namespace {

testing::TestOutcome run_fea_solver() {
    fea_solver::init();
    auto s = fea_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fea_solver::shutdown();
    return testing::ok();
}

const int _reg_fea_solver = testing::register_test(
    "fea_solver",
    "931_fea_solver: stub status check",
    &run_fea_solver);

}
