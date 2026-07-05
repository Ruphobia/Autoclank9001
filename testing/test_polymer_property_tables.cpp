// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/996_polymer_property_tables/polymer_property_tables.hpp"

namespace {

testing::TestOutcome run_polymer_property_tables() {
    polymer_property_tables::init();
    auto s = polymer_property_tables::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polymer_property_tables::shutdown();
    return testing::ok();
}

const int _reg_polymer_property_tables = testing::register_test(
    "polymer_property_tables",
    "996_polymer_property_tables: stub status check",
    &run_polymer_property_tables);

}
