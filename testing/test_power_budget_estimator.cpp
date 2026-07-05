#include "test_runner.hpp"
#include "../modules/221_power_budget_estimator/power_budget_estimator.hpp"

namespace {

testing::TestOutcome run_power_budget_estimator() {
    power_budget_estimator::init();
    auto s = power_budget_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    power_budget_estimator::shutdown();
    return testing::ok();
}

const int _reg_power_budget_estimator = testing::register_test(
    "power_budget_estimator",
    "221_power_budget_estimator: stub status check",
    &run_power_budget_estimator);

}
