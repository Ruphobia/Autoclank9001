#include "test_runner.hpp"
#include "../modules/1060_open_channel_flow_tool/open_channel_flow_tool.hpp"

namespace {

testing::TestOutcome run_open_channel_flow_tool() {
    open_channel_flow_tool::init();
    auto s = open_channel_flow_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    open_channel_flow_tool::shutdown();
    return testing::ok();
}

const int _reg_open_channel_flow_tool = testing::register_test(
    "open_channel_flow_tool",
    "1060_open_channel_flow_tool: stub status check",
    &run_open_channel_flow_tool);

}
