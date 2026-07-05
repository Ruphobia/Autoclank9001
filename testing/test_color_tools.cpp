#include "test_runner.hpp"
#include "../modules/1633_color_tools/color_tools.hpp"

namespace {

testing::TestOutcome run_color_tools() {
    color_tools::init();
    auto s = color_tools::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    color_tools::shutdown();
    return testing::ok();
}

const int _reg_color_tools = testing::register_test(
    "color_tools",
    "1633_color_tools: stub status check",
    &run_color_tools);

}
