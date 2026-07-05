#include "test_runner.hpp"
#include "../modules/2183_behavioral_activation_planner/behavioral_activation_planner.hpp"

namespace {

testing::TestOutcome run_behavioral_activation_planner() {
    behavioral_activation_planner::init();
    auto s = behavioral_activation_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    behavioral_activation_planner::shutdown();
    return testing::ok();
}

const int _reg_behavioral_activation_planner = testing::register_test(
    "behavioral_activation_planner",
    "2183_behavioral_activation_planner: stub status check",
    &run_behavioral_activation_planner);

}
