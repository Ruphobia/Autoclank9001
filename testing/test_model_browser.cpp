// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/672_model_browser/model_browser.hpp"

namespace {

testing::TestOutcome run_model_browser() {
    model_browser::init();
    auto s = model_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    model_browser::shutdown();
    return testing::ok();
}

const int _reg_model_browser = testing::register_test(
    "model_browser",
    "672_model_browser: stub status check",
    &run_model_browser);

}
