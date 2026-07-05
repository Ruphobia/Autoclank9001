#include "test_runner.hpp"
#include "../modules/2181_chronotype_and_circadian_estimator/chronotype_and_circadian_estimator.hpp"

namespace {

testing::TestOutcome run_chronotype_and_circadian_estimator() {
    chronotype_and_circadian_estimator::init();
    auto s = chronotype_and_circadian_estimator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    chronotype_and_circadian_estimator::shutdown();
    return testing::ok();
}

const int _reg_chronotype_and_circadian_estimator = testing::register_test(
    "chronotype_and_circadian_estimator",
    "2181_chronotype_and_circadian_estimator: stub status check",
    &run_chronotype_and_circadian_estimator);

}
