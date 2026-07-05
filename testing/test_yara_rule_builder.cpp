// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/358_yara_rule_builder/yara_rule_builder.hpp"

namespace {

testing::TestOutcome run_yara_rule_builder() {
    yara_rule_builder::init();
    auto s = yara_rule_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    yara_rule_builder::shutdown();
    return testing::ok();
}

const int _reg_yara_rule_builder = testing::register_test(
    "yara_rule_builder",
    "358_yara_rule_builder: stub status check",
    &run_yara_rule_builder);

}
