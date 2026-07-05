// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1364_docking_and_binding_workbench/docking_and_binding_workbench.hpp"

namespace {

testing::TestOutcome run_docking_and_binding_workbench() {
    docking_and_binding_workbench::init();
    auto s = docking_and_binding_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    docking_and_binding_workbench::shutdown();
    return testing::ok();
}

const int _reg_docking_and_binding_workbench = testing::register_test(
    "docking_and_binding_workbench",
    "1364_docking_and_binding_workbench: stub status check",
    &run_docking_and_binding_workbench);

}
