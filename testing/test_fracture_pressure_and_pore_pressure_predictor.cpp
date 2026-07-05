// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1090_fracture_pressure_and_pore_pressure_predictor/fracture_pressure_and_pore_pressure_predictor.hpp"

namespace {

testing::TestOutcome run_fracture_pressure_and_pore_pressure_predictor() {
    fracture_pressure_and_pore_pressure_predictor::init();
    auto s = fracture_pressure_and_pore_pressure_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fracture_pressure_and_pore_pressure_predictor::shutdown();
    return testing::ok();
}

const int _reg_fracture_pressure_and_pore_pressure_predictor = testing::register_test(
    "fracture_pressure_and_pore_pressure_predictor",
    "1090_fracture_pressure_and_pore_pressure_predictor: stub status check",
    &run_fracture_pressure_and_pore_pressure_predictor);

}
