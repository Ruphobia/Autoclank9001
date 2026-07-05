#include "test_runner.hpp"
#include "../modules/1091_block_model_and_ore_grade_estimator/block_model_and_ore_grade_estimator.hpp"

namespace {

testing::TestOutcome run_block_model_and_ore_grade_estimator() {
    block_model_and_ore_grade_estimator::init();
    auto s = block_model_and_ore_grade_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    block_model_and_ore_grade_estimator::shutdown();
    return testing::ok();
}

const int _reg_block_model_and_ore_grade_estimator = testing::register_test(
    "block_model_and_ore_grade_estimator",
    "1091_block_model_and_ore_grade_estimator: stub status check",
    &run_block_model_and_ore_grade_estimator);

}
