// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1785_baker_s_percentage_workbench/baker_s_percentage_workbench.hpp"

namespace {

testing::TestOutcome run_baker_s_percentage_workbench() {
    baker_s_percentage_workbench::init();
    auto s = baker_s_percentage_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    baker_s_percentage_workbench::shutdown();
    return testing::ok();
}

const int _reg_baker_s_percentage_workbench = testing::register_test(
    "baker_s_percentage_workbench",
    "1785_baker_s_percentage_workbench: stub status check",
    &run_baker_s_percentage_workbench);

}
