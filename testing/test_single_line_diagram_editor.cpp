#include "test_runner.hpp"
#include "../modules/1289_single_line_diagram_editor/single_line_diagram_editor.hpp"

namespace {

testing::TestOutcome run_single_line_diagram_editor() {
    single_line_diagram_editor::init();
    auto s = single_line_diagram_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    single_line_diagram_editor::shutdown();
    return testing::ok();
}

const int _reg_single_line_diagram_editor = testing::register_test(
    "single_line_diagram_editor",
    "1289_single_line_diagram_editor: stub status check",
    &run_single_line_diagram_editor);

}
