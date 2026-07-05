// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1507_occupancy_and_capture_recapture/occupancy_and_capture_recapture.hpp"

namespace {

testing::TestOutcome run_occupancy_and_capture_recapture() {
    occupancy_and_capture_recapture::init();
    auto s = occupancy_and_capture_recapture::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    occupancy_and_capture_recapture::shutdown();
    return testing::ok();
}

const int _reg_occupancy_and_capture_recapture = testing::register_test(
    "occupancy_and_capture_recapture",
    "1507_occupancy_and_capture_recapture: stub status check",
    &run_occupancy_and_capture_recapture);

}
