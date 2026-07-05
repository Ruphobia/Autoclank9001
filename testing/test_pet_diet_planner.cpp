#include "test_runner.hpp"
#include "../modules/2017_pet_diet_planner/pet_diet_planner.hpp"

namespace {

testing::TestOutcome run_pet_diet_planner() {
    pet_diet_planner::init();
    auto s = pet_diet_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pet_diet_planner::shutdown();
    return testing::ok();
}

const int _reg_pet_diet_planner = testing::register_test(
    "pet_diet_planner",
    "2017_pet_diet_planner: stub status check",
    &run_pet_diet_planner);

}
