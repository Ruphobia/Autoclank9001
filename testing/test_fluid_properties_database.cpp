#include "test_runner.hpp"
#include "../modules/1028_fluid_properties_database/fluid_properties_database.hpp"

namespace {

testing::TestOutcome run_fluid_properties_database() {
    fluid_properties_database::init();
    auto s = fluid_properties_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fluid_properties_database::shutdown();
    return testing::ok();
}

const int _reg_fluid_properties_database = testing::register_test(
    "fluid_properties_database",
    "1028_fluid_properties_database: stub status check",
    &run_fluid_properties_database);

}
