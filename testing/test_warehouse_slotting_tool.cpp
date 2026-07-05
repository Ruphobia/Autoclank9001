#include "test_runner.hpp"
#include "../modules/488_warehouse_slotting_tool/warehouse_slotting_tool.hpp"

namespace {

testing::TestOutcome run_warehouse_slotting_tool() {
    warehouse_slotting_tool::init();
    auto s = warehouse_slotting_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    warehouse_slotting_tool::shutdown();
    return testing::ok();
}

const int _reg_warehouse_slotting_tool = testing::register_test(
    "warehouse_slotting_tool",
    "488_warehouse_slotting_tool: stub status check",
    &run_warehouse_slotting_tool);

}
