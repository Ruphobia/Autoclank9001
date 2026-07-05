// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1459_nexrad_level_ii_viewer/nexrad_level_ii_viewer.hpp"

namespace {

testing::TestOutcome run_nexrad_level_ii_viewer() {
    nexrad_level_ii_viewer::init();
    auto s = nexrad_level_ii_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nexrad_level_ii_viewer::shutdown();
    return testing::ok();
}

const int _reg_nexrad_level_ii_viewer = testing::register_test(
    "nexrad_level_ii_viewer",
    "1459_nexrad_level_ii_viewer: stub status check",
    &run_nexrad_level_ii_viewer);

}
