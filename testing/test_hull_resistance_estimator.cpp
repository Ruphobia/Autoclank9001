#include "test_runner.hpp"
#include "../modules/1074_hull_resistance_estimator/hull_resistance_estimator.hpp"

namespace {

testing::TestOutcome run_hull_resistance_estimator() {
    hull_resistance_estimator::init();
    auto s = hull_resistance_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hull_resistance_estimator::shutdown();
    return testing::ok();
}

const int _reg_hull_resistance_estimator = testing::register_test(
    "hull_resistance_estimator",
    "1074_hull_resistance_estimator: stub status check",
    &run_hull_resistance_estimator);

}
