// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/272_firewall_rule_editor/firewall_rule_editor.hpp"

namespace {

testing::TestOutcome run_firewall_rule_editor() {
    firewall_rule_editor::init();
    auto s = firewall_rule_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    firewall_rule_editor::shutdown();
    return testing::ok();
}

const int _reg_firewall_rule_editor = testing::register_test(
    "firewall_rule_editor",
    "272_firewall_rule_editor: stub status check",
    &run_firewall_rule_editor);

}
