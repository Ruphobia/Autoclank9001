// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/983_material_safety_database/material_safety_database.hpp"

namespace {

testing::TestOutcome run_material_safety_database() {
    material_safety_database::init();
    auto s = material_safety_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    material_safety_database::shutdown();
    return testing::ok();
}

const int _reg_material_safety_database = testing::register_test(
    "material_safety_database",
    "983_material_safety_database: stub status check",
    &run_material_safety_database);

}
