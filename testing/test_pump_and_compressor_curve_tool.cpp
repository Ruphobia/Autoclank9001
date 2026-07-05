#include "test_runner.hpp"
#include "../modules/1114_pump_and_compressor_curve_tool/pump_and_compressor_curve_tool.hpp"

namespace {

testing::TestOutcome run_pump_and_compressor_curve_tool() {
    pump_and_compressor_curve_tool::init();
    auto s = pump_and_compressor_curve_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pump_and_compressor_curve_tool::shutdown();
    return testing::ok();
}

const int _reg_pump_and_compressor_curve_tool = testing::register_test(
    "pump_and_compressor_curve_tool",
    "1114_pump_and_compressor_curve_tool: stub status check",
    &run_pump_and_compressor_curve_tool);

}
