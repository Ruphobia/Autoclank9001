// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/688_critical_apparatus_builder/critical_apparatus_builder.hpp"

namespace {

testing::TestOutcome run_critical_apparatus_builder() {
    critical_apparatus_builder::init();
    auto s = critical_apparatus_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    critical_apparatus_builder::shutdown();
    return testing::ok();
}

const int _reg_critical_apparatus_builder = testing::register_test(
    "critical_apparatus_builder",
    "688_critical_apparatus_builder: stub status check",
    &run_critical_apparatus_builder);

}
