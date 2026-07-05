// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1209_damper_tuning_workbench/damper_tuning_workbench.hpp"

namespace {

testing::TestOutcome run_damper_tuning_workbench() {
    damper_tuning_workbench::init();
    auto s = damper_tuning_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    damper_tuning_workbench::shutdown();
    return testing::ok();
}

const int _reg_damper_tuning_workbench = testing::register_test(
    "damper_tuning_workbench",
    "1209_damper_tuning_workbench: stub status check",
    &run_damper_tuning_workbench);

}
