#include "test_runner.hpp"
#include "../modules/1868_perspective_grid_overlay/perspective_grid_overlay.hpp"

namespace {

testing::TestOutcome run_perspective_grid_overlay() {
    perspective_grid_overlay::init();
    auto s = perspective_grid_overlay::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    perspective_grid_overlay::shutdown();
    return testing::ok();
}

const int _reg_perspective_grid_overlay = testing::register_test(
    "perspective_grid_overlay",
    "1868_perspective_grid_overlay: stub status check",
    &run_perspective_grid_overlay);

}
