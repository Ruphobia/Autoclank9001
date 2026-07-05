#include "test_runner.hpp"
#include "../modules/562_budget_cash_flow_forecaster/budget_cash_flow_forecaster.hpp"

namespace {

testing::TestOutcome run_budget_cash_flow_forecaster() {
    budget_cash_flow_forecaster::init();
    auto s = budget_cash_flow_forecaster::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    budget_cash_flow_forecaster::shutdown();
    return testing::ok();
}

const int _reg_budget_cash_flow_forecaster = testing::register_test(
    "budget_cash_flow_forecaster",
    "562_budget_cash_flow_forecaster: stub status check",
    &run_budget_cash_flow_forecaster);

}
