#include "test_runner.hpp"
#include "../modules/472_acceptance_sampling_planner/acceptance_sampling_planner.hpp"

namespace {

testing::TestOutcome run_acceptance_sampling_planner() {
    acceptance_sampling_planner::init();
    auto s = acceptance_sampling_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    acceptance_sampling_planner::shutdown();
    return testing::ok();
}

const int _reg_acceptance_sampling_planner = testing::register_test(
    "acceptance_sampling_planner",
    "472_acceptance_sampling_planner: stub status check",
    &run_acceptance_sampling_planner);

}
