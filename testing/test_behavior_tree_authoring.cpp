#include "test_runner.hpp"
#include "../modules/290_behavior_tree_authoring/behavior_tree_authoring.hpp"

namespace {

testing::TestOutcome run_behavior_tree_authoring() {
    behavior_tree_authoring::init();
    auto s = behavior_tree_authoring::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    behavior_tree_authoring::shutdown();
    return testing::ok();
}

const int _reg_behavior_tree_authoring = testing::register_test(
    "behavior_tree_authoring",
    "290_behavior_tree_authoring: stub status check",
    &run_behavior_tree_authoring);

}
