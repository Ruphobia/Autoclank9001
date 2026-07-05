#include "test_runner.hpp"
#include "../modules/1970_poker_equity_solver/poker_equity_solver.hpp"

namespace {

testing::TestOutcome run_poker_equity_solver() {
    poker_equity_solver::init();
    auto s = poker_equity_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    poker_equity_solver::shutdown();
    return testing::ok();
}

const int _reg_poker_equity_solver = testing::register_test(
    "poker_equity_solver",
    "1970_poker_equity_solver: stub status check",
    &run_poker_equity_solver);

}
