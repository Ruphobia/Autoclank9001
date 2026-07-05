// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2097_polar_diagram_workbench/polar_diagram_workbench.hpp"

namespace {

testing::TestOutcome run_polar_diagram_workbench() {
    polar_diagram_workbench::init();
    auto s = polar_diagram_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polar_diagram_workbench::shutdown();
    return testing::ok();
}

const int _reg_polar_diagram_workbench = testing::register_test(
    "polar_diagram_workbench",
    "2097_polar_diagram_workbench: stub status check",
    &run_polar_diagram_workbench);

}
