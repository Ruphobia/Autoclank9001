#include "test_runner.hpp"
#include "../modules/1020_pressure_drop_and_moody_tool/pressure_drop_and_moody_tool.hpp"

namespace {

testing::TestOutcome run_pressure_drop_and_moody_tool() {
    pressure_drop_and_moody_tool::init();
    auto s = pressure_drop_and_moody_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pressure_drop_and_moody_tool::shutdown();
    return testing::ok();
}

const int _reg_pressure_drop_and_moody_tool = testing::register_test(
    "pressure_drop_and_moody_tool",
    "1020_pressure_drop_and_moody_tool: stub status check",
    &run_pressure_drop_and_moody_tool);

}
