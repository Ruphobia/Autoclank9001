#include "test_runner.hpp"
#include "../modules/1427_inter_rater_reliability_calculator/inter_rater_reliability_calculator.hpp"

namespace {

testing::TestOutcome run_inter_rater_reliability_calculator() {
    inter_rater_reliability_calculator::init();
    auto s = inter_rater_reliability_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    inter_rater_reliability_calculator::shutdown();
    return testing::ok();
}

const int _reg_inter_rater_reliability_calculator = testing::register_test(
    "inter_rater_reliability_calculator",
    "1427_inter_rater_reliability_calculator: stub status check",
    &run_inter_rater_reliability_calculator);

}
