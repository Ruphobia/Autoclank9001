#include "test_runner.hpp"
#include "../modules/187_logic_analyzer_bridge/logic_analyzer_bridge.hpp"

namespace {

testing::TestOutcome run_logic_analyzer_bridge() {
    logic_analyzer_bridge::init();
    auto s = logic_analyzer_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    logic_analyzer_bridge::shutdown();
    return testing::ok();
}

const int _reg_logic_analyzer_bridge = testing::register_test(
    "logic_analyzer_bridge",
    "187_logic_analyzer_bridge: stub status check",
    &run_logic_analyzer_bridge);

}
