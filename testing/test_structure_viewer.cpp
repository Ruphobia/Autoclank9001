// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1363_structure_viewer/structure_viewer.hpp"

namespace {

testing::TestOutcome run_structure_viewer() {
    structure_viewer::init();
    auto s = structure_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    structure_viewer::shutdown();
    return testing::ok();
}

const int _reg_structure_viewer = testing::register_test(
    "structure_viewer",
    "1363_structure_viewer: stub status check",
    &run_structure_viewer);

}
