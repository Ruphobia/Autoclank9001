// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1037_load_table_browser/load_table_browser.hpp"

namespace {

testing::TestOutcome run_load_table_browser() {
    load_table_browser::init();
    auto s = load_table_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    load_table_browser::shutdown();
    return testing::ok();
}

const int _reg_load_table_browser = testing::register_test(
    "load_table_browser",
    "1037_load_table_browser: stub status check",
    &run_load_table_browser);

}
