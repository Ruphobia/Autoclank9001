#include "test_runner.hpp"
#include "../modules/1606_crop_rotation_planner/crop_rotation_planner.hpp"

namespace {

testing::TestOutcome run_crop_rotation_planner() {
    crop_rotation_planner::init();
    auto s = crop_rotation_planner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    crop_rotation_planner::shutdown();
    return testing::ok();
}

const int _reg_crop_rotation_planner = testing::register_test(
    "crop_rotation_planner",
    "1606_crop_rotation_planner: stub status check",
    &run_crop_rotation_planner);

}
