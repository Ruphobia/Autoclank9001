#include "test_runner.hpp"
#include "../modules/926_mesh_editor/mesh_editor.hpp"

namespace {

testing::TestOutcome run_mesh_editor() {
    mesh_editor::init();
    auto s = mesh_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mesh_editor::shutdown();
    return testing::ok();
}

const int _reg_mesh_editor = testing::register_test(
    "mesh_editor",
    "926_mesh_editor: stub status check",
    &run_mesh_editor);

}
