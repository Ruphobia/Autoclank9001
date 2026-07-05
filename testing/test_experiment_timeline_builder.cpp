#include "test_runner.hpp"
#include "../modules/1419_experiment_timeline_builder/experiment_timeline_builder.hpp"

namespace {

testing::TestOutcome run_experiment_timeline_builder() {
    experiment_timeline_builder::init();
    auto s = experiment_timeline_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    experiment_timeline_builder::shutdown();
    return testing::ok();
}

const int _reg_experiment_timeline_builder = testing::register_test(
    "experiment_timeline_builder",
    "1419_experiment_timeline_builder: stub status check",
    &run_experiment_timeline_builder);

}
