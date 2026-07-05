#include "test_runner.hpp"
#include "../modules/663_experiment_tracker/experiment_tracker.hpp"

namespace {

testing::TestOutcome run_experiment_tracker() {
    experiment_tracker::init();
    auto s = experiment_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    experiment_tracker::shutdown();
    return testing::ok();
}

const int _reg_experiment_tracker = testing::register_test(
    "experiment_tracker",
    "663_experiment_tracker: stub status check",
    &run_experiment_tracker);

}
