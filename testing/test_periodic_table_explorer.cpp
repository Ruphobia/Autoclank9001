// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1306_periodic_table_explorer/periodic_table_explorer.hpp"

namespace {

testing::TestOutcome run_periodic_table_explorer() {
    periodic_table_explorer::init();
    auto s = periodic_table_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    periodic_table_explorer::shutdown();
    return testing::ok();
}

const int _reg_periodic_table_explorer = testing::register_test(
    "periodic_table_explorer",
    "1306_periodic_table_explorer: stub status check",
    &run_periodic_table_explorer);

}
