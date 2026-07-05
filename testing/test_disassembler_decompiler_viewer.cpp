#include "test_runner.hpp"
#include "../modules/359_disassembler_decompiler_viewer/disassembler_decompiler_viewer.hpp"

namespace {

testing::TestOutcome run_disassembler_decompiler_viewer() {
    disassembler_decompiler_viewer::init();
    auto s = disassembler_decompiler_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    disassembler_decompiler_viewer::shutdown();
    return testing::ok();
}

const int _reg_disassembler_decompiler_viewer = testing::register_test(
    "disassembler_decompiler_viewer",
    "359_disassembler_decompiler_viewer: stub status check",
    &run_disassembler_decompiler_viewer);

}
