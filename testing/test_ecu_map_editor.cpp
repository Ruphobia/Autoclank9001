// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1201_ecu_map_editor/ecu_map_editor.hpp"

namespace {

testing::TestOutcome run_ecu_map_editor() {
    ecu_map_editor::init();
    auto s = ecu_map_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ecu_map_editor::shutdown();
    return testing::ok();
}

const int _reg_ecu_map_editor = testing::register_test(
    "ecu_map_editor",
    "1201_ecu_map_editor: stub status check",
    &run_ecu_map_editor);

}
