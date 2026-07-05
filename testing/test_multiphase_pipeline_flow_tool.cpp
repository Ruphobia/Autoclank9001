#include "test_runner.hpp"
#include "../modules/1115_multiphase_pipeline_flow_tool/multiphase_pipeline_flow_tool.hpp"

namespace {

testing::TestOutcome run_multiphase_pipeline_flow_tool() {
    multiphase_pipeline_flow_tool::init();
    auto s = multiphase_pipeline_flow_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multiphase_pipeline_flow_tool::shutdown();
    return testing::ok();
}

const int _reg_multiphase_pipeline_flow_tool = testing::register_test(
    "multiphase_pipeline_flow_tool",
    "1115_multiphase_pipeline_flow_tool: stub status check",
    &run_multiphase_pipeline_flow_tool);

}
