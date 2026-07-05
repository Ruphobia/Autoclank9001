// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/630_metadata_editor/metadata_editor.hpp"

namespace {

testing::TestOutcome run_metadata_editor() {
    metadata_editor::init();
    auto s = metadata_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    metadata_editor::shutdown();
    return testing::ok();
}

const int _reg_metadata_editor = testing::register_test(
    "metadata_editor",
    "630_metadata_editor: stub status check",
    &run_metadata_editor);

}
