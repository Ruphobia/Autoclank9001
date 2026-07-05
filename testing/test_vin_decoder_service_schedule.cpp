// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1225_vin_decoder_service_schedule/vin_decoder_service_schedule.hpp"

namespace {

testing::TestOutcome run_vin_decoder_service_schedule() {
    vin_decoder_service_schedule::init();
    auto s = vin_decoder_service_schedule::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vin_decoder_service_schedule::shutdown();
    return testing::ok();
}

const int _reg_vin_decoder_service_schedule = testing::register_test(
    "vin_decoder_service_schedule",
    "1225_vin_decoder_service_schedule: stub status check",
    &run_vin_decoder_service_schedule);

}
