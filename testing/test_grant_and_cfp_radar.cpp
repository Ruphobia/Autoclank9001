// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/654_grant_and_cfp_radar/grant_and_cfp_radar.hpp"

namespace {

testing::TestOutcome run_grant_and_cfp_radar() {
    grant_and_cfp_radar::init();
    auto s = grant_and_cfp_radar::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    grant_and_cfp_radar::shutdown();
    return testing::ok();
}

const int _reg_grant_and_cfp_radar = testing::register_test(
    "grant_and_cfp_radar",
    "654_grant_and_cfp_radar: stub status check",
    &run_grant_and_cfp_radar);

}
