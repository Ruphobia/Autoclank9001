#include "test_runner.hpp"
#include "../modules/1078_mooring_catenary_solver/mooring_catenary_solver.hpp"

namespace {

testing::TestOutcome run_mooring_catenary_solver() {
    mooring_catenary_solver::init();
    auto s = mooring_catenary_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mooring_catenary_solver::shutdown();
    return testing::ok();
}

const int _reg_mooring_catenary_solver = testing::register_test(
    "mooring_catenary_solver",
    "1078_mooring_catenary_solver: stub status check",
    &run_mooring_catenary_solver);

}
