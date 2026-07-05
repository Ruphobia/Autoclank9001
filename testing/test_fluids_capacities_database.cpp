#include "test_runner.hpp"
#include "../modules/1231_fluids_capacities_database/fluids_capacities_database.hpp"

namespace {

testing::TestOutcome run_fluids_capacities_database() {
    fluids_capacities_database::init();
    auto s = fluids_capacities_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fluids_capacities_database::shutdown();
    return testing::ok();
}

const int _reg_fluids_capacities_database = testing::register_test(
    "fluids_capacities_database",
    "1231_fluids_capacities_database: stub status check",
    &run_fluids_capacities_database);

}
