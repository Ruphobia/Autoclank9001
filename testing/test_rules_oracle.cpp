// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1963_rules_oracle/rules_oracle.hpp"

namespace {

testing::TestOutcome run_rules_oracle() {
    rules_oracle::init();
    auto s = rules_oracle::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rules_oracle::shutdown();
    return testing::ok();
}

const int _reg_rules_oracle = testing::register_test(
    "rules_oracle",
    "1963_rules_oracle: stub status check",
    &run_rules_oracle);

}
