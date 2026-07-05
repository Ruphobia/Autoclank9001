// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1585_seismic_trace_viewer/seismic_trace_viewer.hpp"

namespace {

testing::TestOutcome run_seismic_trace_viewer() {
    seismic_trace_viewer::init();
    auto s = seismic_trace_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    seismic_trace_viewer::shutdown();
    return testing::ok();
}

const int _reg_seismic_trace_viewer = testing::register_test(
    "seismic_trace_viewer",
    "1585_seismic_trace_viewer: stub status check",
    &run_seismic_trace_viewer);

}
