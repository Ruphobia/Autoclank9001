#include "test_runner.hpp"
#include "../modules/1815_salt_and_brine_chemistry_solver/salt_and_brine_chemistry_solver.hpp"

namespace {

testing::TestOutcome run_salt_and_brine_chemistry_solver() {
    salt_and_brine_chemistry_solver::init();
    auto s = salt_and_brine_chemistry_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    salt_and_brine_chemistry_solver::shutdown();
    return testing::ok();
}

const int _reg_salt_and_brine_chemistry_solver = testing::register_test(
    "salt_and_brine_chemistry_solver",
    "1815_salt_and_brine_chemistry_solver: stub status check",
    &run_salt_and_brine_chemistry_solver);

}
