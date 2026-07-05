#include "test_runner.hpp"
#include "../modules/492_multi_echelon_inventory_simulator/multi_echelon_inventory_simulator.hpp"

namespace {

testing::TestOutcome run_multi_echelon_inventory_simulator() {
    multi_echelon_inventory_simulator::init();
    auto s = multi_echelon_inventory_simulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multi_echelon_inventory_simulator::shutdown();
    return testing::ok();
}

const int _reg_multi_echelon_inventory_simulator = testing::register_test(
    "multi_echelon_inventory_simulator",
    "492_multi_echelon_inventory_simulator: stub status check",
    &run_multi_echelon_inventory_simulator);

}
