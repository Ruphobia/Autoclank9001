#include "test_runner.hpp"
#include "../modules/1105_decay_heat_estimator/decay_heat_estimator.hpp"

namespace {

testing::TestOutcome run_decay_heat_estimator() {
    decay_heat_estimator::init();
    auto s = decay_heat_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    decay_heat_estimator::shutdown();
    return testing::ok();
}

const int _reg_decay_heat_estimator = testing::register_test(
    "decay_heat_estimator",
    "1105_decay_heat_estimator: stub status check",
    &run_decay_heat_estimator);

}
