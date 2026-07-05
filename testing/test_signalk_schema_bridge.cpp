#include "test_runner.hpp"
#include "../modules/2100_signalk_schema_bridge/signalk_schema_bridge.hpp"

namespace {

testing::TestOutcome run_signalk_schema_bridge() {
    signalk_schema_bridge::init();
    auto s = signalk_schema_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    signalk_schema_bridge::shutdown();
    return testing::ok();
}

const int _reg_signalk_schema_bridge = testing::register_test(
    "signalk_schema_bridge",
    "2100_signalk_schema_bridge: stub status check",
    &run_signalk_schema_bridge);

}
