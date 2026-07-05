// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1954_campaign_log/campaign_log.hpp"

namespace {

testing::TestOutcome run_campaign_log() {
    campaign_log::init();
    auto s = campaign_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    campaign_log::shutdown();
    return testing::ok();
}

const int _reg_campaign_log = testing::register_test(
    "campaign_log",
    "1954_campaign_log: stub status check",
    &run_campaign_log);

}
