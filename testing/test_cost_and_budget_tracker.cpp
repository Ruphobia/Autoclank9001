#include "test_runner.hpp"
#include "../modules/458_cost_and_budget_tracker/cost_and_budget_tracker.hpp"

namespace {

testing::TestOutcome run_cost_and_budget_tracker() {
    cost_and_budget_tracker::init();
    auto s = cost_and_budget_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cost_and_budget_tracker::shutdown();
    return testing::ok();
}

const int _reg_cost_and_budget_tracker = testing::register_test(
    "cost_and_budget_tracker",
    "458_cost_and_budget_tracker: stub status check",
    &run_cost_and_budget_tracker);

}
