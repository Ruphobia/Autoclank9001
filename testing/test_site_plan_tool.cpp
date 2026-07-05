// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1049_site_plan_tool/site_plan_tool.hpp"

namespace {

testing::TestOutcome run_site_plan_tool() {
    site_plan_tool::init();
    auto s = site_plan_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    site_plan_tool::shutdown();
    return testing::ok();
}

const int _reg_site_plan_tool = testing::register_test(
    "site_plan_tool",
    "1049_site_plan_tool: stub status check",
    &run_site_plan_tool);

}
