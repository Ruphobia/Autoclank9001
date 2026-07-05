#include "test_runner.hpp"
#include "../modules/1182_rig_control_bridge/rig_control_bridge.hpp"

namespace {

testing::TestOutcome run_rig_control_bridge() {
    rig_control_bridge::init();
    auto s = rig_control_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rig_control_bridge::shutdown();
    return testing::ok();
}

const int _reg_rig_control_bridge = testing::register_test(
    "rig_control_bridge",
    "1182_rig_control_bridge: stub status check",
    &run_rig_control_bridge);

}
