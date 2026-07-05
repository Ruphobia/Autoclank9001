#include "test_runner.hpp"
#include "../modules/786_phoneme_inventory_editor/phoneme_inventory_editor.hpp"

namespace {

testing::TestOutcome run_phoneme_inventory_editor() {
    phoneme_inventory_editor::init();
    auto s = phoneme_inventory_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    phoneme_inventory_editor::shutdown();
    return testing::ok();
}

const int _reg_phoneme_inventory_editor = testing::register_test(
    "phoneme_inventory_editor",
    "786_phoneme_inventory_editor: stub status check",
    &run_phoneme_inventory_editor);

}
