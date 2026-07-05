// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1551_ais_vessel_decoder/ais_vessel_decoder.hpp"

namespace {

testing::TestOutcome run_ais_vessel_decoder() {
    ais_vessel_decoder::init();
    auto s = ais_vessel_decoder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ais_vessel_decoder::shutdown();
    return testing::ok();
}

const int _reg_ais_vessel_decoder = testing::register_test(
    "ais_vessel_decoder",
    "1551_ais_vessel_decoder: stub status check",
    &run_ais_vessel_decoder);

}
