// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1173_notam_and_tfr_browser/notam_and_tfr_browser.hpp"

namespace {

testing::TestOutcome run_notam_and_tfr_browser() {
    notam_and_tfr_browser::init();
    auto s = notam_and_tfr_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    notam_and_tfr_browser::shutdown();
    return testing::ok();
}

const int _reg_notam_and_tfr_browser = testing::register_test(
    "notam_and_tfr_browser",
    "1173_notam_and_tfr_browser: stub status check",
    &run_notam_and_tfr_browser);

}
