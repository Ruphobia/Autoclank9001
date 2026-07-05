#include "test_runner.hpp"
#include "../modules/482_newsvendor_solver/newsvendor_solver.hpp"

namespace {

testing::TestOutcome run_newsvendor_solver() {
    newsvendor_solver::init();
    auto s = newsvendor_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    newsvendor_solver::shutdown();
    return testing::ok();
}

const int _reg_newsvendor_solver = testing::register_test(
    "newsvendor_solver",
    "482_newsvendor_solver: stub status check",
    &run_newsvendor_solver);

}
