// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1905_anatomy_and_risk_atlas/anatomy_and_risk_atlas.hpp"

namespace {

testing::TestOutcome run_anatomy_and_risk_atlas() {
    anatomy_and_risk_atlas::init();
    auto s = anatomy_and_risk_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anatomy_and_risk_atlas::shutdown();
    return testing::ok();
}

const int _reg_anatomy_and_risk_atlas = testing::register_test(
    "anatomy_and_risk_atlas",
    "1905_anatomy_and_risk_atlas: stub status check",
    &run_anatomy_and_risk_atlas);

}
