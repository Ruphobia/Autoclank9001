// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/987_phase_diagram_viewer/phase_diagram_viewer.hpp"

namespace {

testing::TestOutcome run_phase_diagram_viewer() {
    phase_diagram_viewer::init();
    auto s = phase_diagram_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    phase_diagram_viewer::shutdown();
    return testing::ok();
}

const int _reg_phase_diagram_viewer = testing::register_test(
    "phase_diagram_viewer",
    "987_phase_diagram_viewer: stub status check",
    &run_phase_diagram_viewer);

}
