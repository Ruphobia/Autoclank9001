// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2187_hrv_and_biofeedback_viewer/hrv_and_biofeedback_viewer.hpp"

namespace {

testing::TestOutcome run_hrv_and_biofeedback_viewer() {
    hrv_and_biofeedback_viewer::init();
    auto s = hrv_and_biofeedback_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hrv_and_biofeedback_viewer::shutdown();
    return testing::ok();
}

const int _reg_hrv_and_biofeedback_viewer = testing::register_test(
    "hrv_and_biofeedback_viewer",
    "2187_hrv_and_biofeedback_viewer: stub status check",
    &run_hrv_and_biofeedback_viewer);

}
