// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/995_pourbaix_diagram_viewer/pourbaix_diagram_viewer.hpp"

namespace {

testing::TestOutcome run_pourbaix_diagram_viewer() {
    pourbaix_diagram_viewer::init();
    auto s = pourbaix_diagram_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pourbaix_diagram_viewer::shutdown();
    return testing::ok();
}

const int _reg_pourbaix_diagram_viewer = testing::register_test(
    "pourbaix_diagram_viewer",
    "995_pourbaix_diagram_viewer: stub status check",
    &run_pourbaix_diagram_viewer);

}
