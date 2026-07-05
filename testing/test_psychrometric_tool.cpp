#include "test_runner.hpp"
#include "../modules/2025_psychrometric_tool/psychrometric_tool.hpp"

namespace {

testing::TestOutcome run_psychrometric_tool() {
    psychrometric_tool::init();
    auto s = psychrometric_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    psychrometric_tool::shutdown();
    return testing::ok();
}

const int _reg_psychrometric_tool = testing::register_test(
    "psychrometric_tool",
    "2025_psychrometric_tool: stub status check",
    &run_psychrometric_tool);

}
