#include "test_runner.hpp"
#include "../modules/1383_rna_structure_and_riboswitch_tool/rna_structure_and_riboswitch_tool.hpp"

namespace {

testing::TestOutcome run_rna_structure_and_riboswitch_tool() {
    rna_structure_and_riboswitch_tool::init();
    auto s = rna_structure_and_riboswitch_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rna_structure_and_riboswitch_tool::shutdown();
    return testing::ok();
}

const int _reg_rna_structure_and_riboswitch_tool = testing::register_test(
    "rna_structure_and_riboswitch_tool",
    "1383_rna_structure_and_riboswitch_tool: stub status check",
    &run_rna_structure_and_riboswitch_tool);

}
