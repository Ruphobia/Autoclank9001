#include "test_runner.hpp"
#include "../modules/1441_ensemble_plan_analyzer/ensemble_plan_analyzer.hpp"

namespace {

testing::TestOutcome run_ensemble_plan_analyzer() {
    ensemble_plan_analyzer::init();
    auto s = ensemble_plan_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ensemble_plan_analyzer::shutdown();
    return testing::ok();
}

const int _reg_ensemble_plan_analyzer = testing::register_test(
    "ensemble_plan_analyzer",
    "1441_ensemble_plan_analyzer: stub status check",
    &run_ensemble_plan_analyzer);

}
