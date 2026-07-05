#include "test_runner.hpp"
#include "../modules/1243_fuel_economy_cost_of_ownership_log/fuel_economy_cost_of_ownership_log.hpp"

namespace {

testing::TestOutcome run_fuel_economy_cost_of_ownership_log() {
    fuel_economy_cost_of_ownership_log::init();
    auto s = fuel_economy_cost_of_ownership_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fuel_economy_cost_of_ownership_log::shutdown();
    return testing::ok();
}

const int _reg_fuel_economy_cost_of_ownership_log = testing::register_test(
    "fuel_economy_cost_of_ownership_log",
    "1243_fuel_economy_cost_of_ownership_log: stub status check",
    &run_fuel_economy_cost_of_ownership_log);

}
