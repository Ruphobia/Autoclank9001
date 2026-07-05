// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1212_tire_pressure_and_temperature_logger/tire_pressure_and_temperature_logger.hpp"

namespace {

testing::TestOutcome run_tire_pressure_and_temperature_logger() {
    tire_pressure_and_temperature_logger::init();
    auto s = tire_pressure_and_temperature_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tire_pressure_and_temperature_logger::shutdown();
    return testing::ok();
}

const int _reg_tire_pressure_and_temperature_logger = testing::register_test(
    "tire_pressure_and_temperature_logger",
    "1212_tire_pressure_and_temperature_logger: stub status check",
    &run_tire_pressure_and_temperature_logger);

}
