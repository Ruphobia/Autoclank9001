// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1623_vector_editor/vector_editor.hpp"

namespace {

testing::TestOutcome run_vector_editor() {
    vector_editor::init();
    auto s = vector_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vector_editor::shutdown();
    return testing::ok();
}

const int _reg_vector_editor = testing::register_test(
    "vector_editor",
    "1623_vector_editor: stub status check",
    &run_vector_editor);

}
