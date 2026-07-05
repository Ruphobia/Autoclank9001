// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/540_mortality_table_browser/mortality_table_browser.hpp"

namespace {

testing::TestOutcome run_mortality_table_browser() {
    mortality_table_browser::init();
    auto s = mortality_table_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mortality_table_browser::shutdown();
    return testing::ok();
}

const int _reg_mortality_table_browser = testing::register_test(
    "mortality_table_browser",
    "540_mortality_table_browser: stub status check",
    &run_mortality_table_browser);

}
