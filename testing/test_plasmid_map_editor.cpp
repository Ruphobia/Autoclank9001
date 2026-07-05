#include "test_runner.hpp"
#include "../modules/1329_plasmid_map_editor/plasmid_map_editor.hpp"

namespace {

testing::TestOutcome run_plasmid_map_editor() {
    plasmid_map_editor::init();
    auto s = plasmid_map_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plasmid_map_editor::shutdown();
    return testing::ok();
}

const int _reg_plasmid_map_editor = testing::register_test(
    "plasmid_map_editor",
    "1329_plasmid_map_editor: stub status check",
    &run_plasmid_map_editor);

}
