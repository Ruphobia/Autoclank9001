// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/813_comparative_method_workbench/comparative_method_workbench.hpp"

namespace {

testing::TestOutcome run_comparative_method_workbench() {
    comparative_method_workbench::init();
    auto s = comparative_method_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comparative_method_workbench::shutdown();
    return testing::ok();
}

const int _reg_comparative_method_workbench = testing::register_test(
    "comparative_method_workbench",
    "813_comparative_method_workbench: stub status check",
    &run_comparative_method_workbench);

}
