// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1274_tilt_azimuth_optimizer/tilt_azimuth_optimizer.hpp"

namespace {

testing::TestOutcome run_tilt_azimuth_optimizer() {
    tilt_azimuth_optimizer::init();
    auto s = tilt_azimuth_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tilt_azimuth_optimizer::shutdown();
    return testing::ok();
}

const int _reg_tilt_azimuth_optimizer = testing::register_test(
    "tilt_azimuth_optimizer",
    "1274_tilt_azimuth_optimizer: stub status check",
    &run_tilt_azimuth_optimizer);

}
