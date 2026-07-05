#include "test_runner.hpp"
#include "../modules/075_embedded_browser_tool/embedded_browser_tool.hpp"

namespace {

testing::TestOutcome run_embedded_browser_tool() {
    embedded_browser_tool::init();
    auto s = embedded_browser_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    embedded_browser_tool::shutdown();
    return testing::ok();
}

const int _reg_embedded_browser_tool = testing::register_test(
    "embedded_browser_tool",
    "075_embedded_browser_tool: stub status check",
    &run_embedded_browser_tool);

}
