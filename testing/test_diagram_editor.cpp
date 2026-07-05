#include "test_runner.hpp"
#include "../modules/427_diagram_editor/diagram_editor.hpp"

namespace {

testing::TestOutcome run_diagram_editor() {
    diagram_editor::init();
    auto s = diagram_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    diagram_editor::shutdown();
    return testing::ok();
}

const int _reg_diagram_editor = testing::register_test(
    "diagram_editor",
    "427_diagram_editor: stub status check",
    &run_diagram_editor);

}
