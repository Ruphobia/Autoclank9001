#include "test_runner.hpp"
#include "../modules/1099_resource_classification_tool/resource_classification_tool.hpp"

namespace {

testing::TestOutcome run_resource_classification_tool() {
    resource_classification_tool::init();
    auto s = resource_classification_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    resource_classification_tool::shutdown();
    return testing::ok();
}

const int _reg_resource_classification_tool = testing::register_test(
    "resource_classification_tool",
    "1099_resource_classification_tool: stub status check",
    &run_resource_classification_tool);

}
