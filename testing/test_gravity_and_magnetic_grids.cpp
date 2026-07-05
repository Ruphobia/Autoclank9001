#include "test_runner.hpp"
#include "../modules/1596_gravity_and_magnetic_grids/gravity_and_magnetic_grids.hpp"

namespace {

testing::TestOutcome run_gravity_and_magnetic_grids() {
    gravity_and_magnetic_grids::init();
    auto s = gravity_and_magnetic_grids::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gravity_and_magnetic_grids::shutdown();
    return testing::ok();
}

const int _reg_gravity_and_magnetic_grids = testing::register_test(
    "gravity_and_magnetic_grids",
    "1596_gravity_and_magnetic_grids: stub status check",
    &run_gravity_and_magnetic_grids);

}
