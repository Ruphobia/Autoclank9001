// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/570_net_worth_dashboard/net_worth_dashboard.hpp"

namespace {

testing::TestOutcome run_net_worth_dashboard() {
    net_worth_dashboard::init();
    auto s = net_worth_dashboard::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    net_worth_dashboard::shutdown();
    return testing::ok();
}

const int _reg_net_worth_dashboard = testing::register_test(
    "net_worth_dashboard",
    "570_net_worth_dashboard: stub status check",
    &run_net_worth_dashboard);

}
