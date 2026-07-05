// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1117_materials_property_database/materials_property_database.hpp"

namespace {

testing::TestOutcome run_materials_property_database() {
    materials_property_database::init();
    auto s = materials_property_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    materials_property_database::shutdown();
    return testing::ok();
}

const int _reg_materials_property_database = testing::register_test(
    "materials_property_database",
    "1117_materials_property_database: stub status check",
    &run_materials_property_database);

}
