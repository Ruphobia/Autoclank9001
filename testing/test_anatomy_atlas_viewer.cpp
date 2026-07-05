// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1402_anatomy_atlas_viewer/anatomy_atlas_viewer.hpp"

namespace {

testing::TestOutcome run_anatomy_atlas_viewer() {
    anatomy_atlas_viewer::init();
    auto s = anatomy_atlas_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anatomy_atlas_viewer::shutdown();
    return testing::ok();
}

const int _reg_anatomy_atlas_viewer = testing::register_test(
    "anatomy_atlas_viewer",
    "1402_anatomy_atlas_viewer: stub status check",
    &run_anatomy_atlas_viewer);

}
