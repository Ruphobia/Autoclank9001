// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1077_hydrostatics_and_stability_calculator/hydrostatics_and_stability_calculator.hpp"

namespace {

testing::TestOutcome run_hydrostatics_and_stability_calculator() {
    hydrostatics_and_stability_calculator::init();
    auto s = hydrostatics_and_stability_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hydrostatics_and_stability_calculator::shutdown();
    return testing::ok();
}

const int _reg_hydrostatics_and_stability_calculator = testing::register_test(
    "hydrostatics_and_stability_calculator",
    "1077_hydrostatics_and_stability_calculator: stub status check",
    &run_hydrostatics_and_stability_calculator);

}
