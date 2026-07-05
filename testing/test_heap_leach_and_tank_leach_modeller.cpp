#include "test_runner.hpp"
#include "../modules/1100_heap_leach_and_tank_leach_modeller/heap_leach_and_tank_leach_modeller.hpp"

namespace {

testing::TestOutcome run_heap_leach_and_tank_leach_modeller() {
    heap_leach_and_tank_leach_modeller::init();
    auto s = heap_leach_and_tank_leach_modeller::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    heap_leach_and_tank_leach_modeller::shutdown();
    return testing::ok();
}

const int _reg_heap_leach_and_tank_leach_modeller = testing::register_test(
    "heap_leach_and_tank_leach_modeller",
    "1100_heap_leach_and_tank_leach_modeller: stub status check",
    &run_heap_leach_and_tank_leach_modeller);

}
