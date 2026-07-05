#include "test_runner.hpp"
#include "../modules/1699_cloth_and_hair_sim/cloth_and_hair_sim.hpp"

namespace {

testing::TestOutcome run_cloth_and_hair_sim() {
    cloth_and_hair_sim::init();
    auto s = cloth_and_hair_sim::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cloth_and_hair_sim::shutdown();
    return testing::ok();
}

const int _reg_cloth_and_hair_sim = testing::register_test(
    "cloth_and_hair_sim",
    "1699_cloth_and_hair_sim: stub status check",
    &run_cloth_and_hair_sim);

}
