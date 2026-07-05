#include "test_runner.hpp"
#include "../modules/1295_molecule_editor/molecule_editor.hpp"

namespace {

testing::TestOutcome run_molecule_editor() {
    molecule_editor::init();
    auto s = molecule_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    molecule_editor::shutdown();
    return testing::ok();
}

const int _reg_molecule_editor = testing::register_test(
    "molecule_editor",
    "1295_molecule_editor: stub status check",
    &run_molecule_editor);

}
