// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/276_reverse_proxy_editor/reverse_proxy_editor.hpp"

namespace {

testing::TestOutcome run_reverse_proxy_editor() {
    reverse_proxy_editor::init();
    auto s = reverse_proxy_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reverse_proxy_editor::shutdown();
    return testing::ok();
}

const int _reg_reverse_proxy_editor = testing::register_test(
    "reverse_proxy_editor",
    "276_reverse_proxy_editor: stub status check",
    &run_reverse_proxy_editor);

}
