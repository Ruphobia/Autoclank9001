// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/328_frustum_and_fov_visualizer/frustum_and_fov_visualizer.hpp"

namespace {

testing::TestOutcome run_frustum_and_fov_visualizer() {
    frustum_and_fov_visualizer::init();
    auto s = frustum_and_fov_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frustum_and_fov_visualizer::shutdown();
    return testing::ok();
}

const int _reg_frustum_and_fov_visualizer = testing::register_test(
    "frustum_and_fov_visualizer",
    "328_frustum_and_fov_visualizer: stub status check",
    &run_frustum_and_fov_visualizer);

}
