// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1667_lens_database_browser/lens_database_browser.hpp"

namespace {

testing::TestOutcome run_lens_database_browser() {
    lens_database_browser::init();
    auto s = lens_database_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lens_database_browser::shutdown();
    return testing::ok();
}

const int _reg_lens_database_browser = testing::register_test(
    "lens_database_browser",
    "1667_lens_database_browser: stub status check",
    &run_lens_database_browser);

}
