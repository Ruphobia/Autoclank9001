// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1587_stereonet/stereonet.hpp"

namespace {

testing::TestOutcome run_stereonet() {
    stereonet::init();
    auto s = stereonet::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    stereonet::shutdown();
    return testing::ok();
}

const int _reg_stereonet = testing::register_test(
    "stereonet",
    "1587_stereonet: stub status check",
    &run_stereonet);

}
