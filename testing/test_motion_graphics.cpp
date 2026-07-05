#include "test_runner.hpp"
#include "../modules/1649_motion_graphics/motion_graphics.hpp"

namespace {

testing::TestOutcome run_motion_graphics() {
    motion_graphics::init();
    auto s = motion_graphics::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    motion_graphics::shutdown();
    return testing::ok();
}

const int _reg_motion_graphics = testing::register_test(
    "motion_graphics",
    "1649_motion_graphics: stub status check",
    &run_motion_graphics);

}
