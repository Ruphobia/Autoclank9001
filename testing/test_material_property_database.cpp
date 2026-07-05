#include "test_runner.hpp"
#include "../modules/921_material_property_database/material_property_database.hpp"

namespace {

testing::TestOutcome run_material_property_database() {
    material_property_database::init();
    auto s = material_property_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    material_property_database::shutdown();
    return testing::ok();
}

const int _reg_material_property_database = testing::register_test(
    "material_property_database",
    "921_material_property_database: stub status check",
    &run_material_property_database);

}
