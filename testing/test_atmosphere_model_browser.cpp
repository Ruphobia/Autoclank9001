// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1137_atmosphere_model_browser/atmosphere_model_browser.hpp"

namespace {

testing::TestOutcome run_atmosphere_model_browser() {
    atmosphere_model_browser::init();
    auto s = atmosphere_model_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    atmosphere_model_browser::shutdown();
    return testing::ok();
}

const int _reg_atmosphere_model_browser = testing::register_test(
    "atmosphere_model_browser",
    "1137_atmosphere_model_browser: stub status check",
    &run_atmosphere_model_browser);

}
