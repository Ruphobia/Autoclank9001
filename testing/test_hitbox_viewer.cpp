#include "test_runner.hpp"
#include "../modules/2000_hitbox_viewer/hitbox_viewer.hpp"

namespace {

testing::TestOutcome run_hitbox_viewer() {
    hitbox_viewer::init();
    auto s = hitbox_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hitbox_viewer::shutdown();
    return testing::ok();
}

const int _reg_hitbox_viewer = testing::register_test(
    "hitbox_viewer",
    "2000_hitbox_viewer: stub status check",
    &run_hitbox_viewer);

}
