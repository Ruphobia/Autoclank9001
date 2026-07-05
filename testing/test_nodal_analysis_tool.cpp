#include "test_runner.hpp"
#include "../modules/1085_nodal_analysis_tool/nodal_analysis_tool.hpp"

namespace {

testing::TestOutcome run_nodal_analysis_tool() {
    nodal_analysis_tool::init();
    auto s = nodal_analysis_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nodal_analysis_tool::shutdown();
    return testing::ok();
}

const int _reg_nodal_analysis_tool = testing::register_test(
    "nodal_analysis_tool",
    "1085_nodal_analysis_tool: stub status check",
    &run_nodal_analysis_tool);

}
