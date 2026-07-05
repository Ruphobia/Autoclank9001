#include "test_runner.hpp"
#include "../modules/2012_par_and_ppfd_planner/par_and_ppfd_planner.hpp"

namespace {

testing::TestOutcome run_par_and_ppfd_planner() {
    par_and_ppfd_planner::init();
    auto s = par_and_ppfd_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    par_and_ppfd_planner::shutdown();
    return testing::ok();
}

const int _reg_par_and_ppfd_planner = testing::register_test(
    "par_and_ppfd_planner",
    "2012_par_and_ppfd_planner: stub status check",
    &run_par_and_ppfd_planner);

}
