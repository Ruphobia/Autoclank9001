#include "test_runner.hpp"
#include "../modules/973_laser_material_profile_library/laser_material_profile_library.hpp"

namespace {

testing::TestOutcome run_laser_material_profile_library() {
    laser_material_profile_library::init();
    auto s = laser_material_profile_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    laser_material_profile_library::shutdown();
    return testing::ok();
}

const int _reg_laser_material_profile_library = testing::register_test(
    "laser_material_profile_library",
    "973_laser_material_profile_library: stub status check",
    &run_laser_material_profile_library);

}
