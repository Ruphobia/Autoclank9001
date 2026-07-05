// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/969_pressure_advance_and_input_shaper_tuner/pressure_advance_and_input_shaper_tuner.hpp"

namespace {

testing::TestOutcome run_pressure_advance_and_input_shaper_tuner() {
    pressure_advance_and_input_shaper_tuner::init();
    auto s = pressure_advance_and_input_shaper_tuner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pressure_advance_and_input_shaper_tuner::shutdown();
    return testing::ok();
}

const int _reg_pressure_advance_and_input_shaper_tuner = testing::register_test(
    "pressure_advance_and_input_shaper_tuner",
    "969_pressure_advance_and_input_shaper_tuner: stub status check",
    &run_pressure_advance_and_input_shaper_tuner);

}
