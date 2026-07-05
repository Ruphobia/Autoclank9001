#include "test_runner.hpp"
#include "../modules/992_strength_estimator/strength_estimator.hpp"

namespace {

testing::TestOutcome run_strength_estimator() {
    strength_estimator::init();
    auto s = strength_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    strength_estimator::shutdown();
    return testing::ok();
}

const int _reg_strength_estimator = testing::register_test(
    "strength_estimator",
    "992_strength_estimator: stub status check",
    &run_strength_estimator);

}
