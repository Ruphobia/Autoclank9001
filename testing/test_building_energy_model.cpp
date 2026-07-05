// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1045_building_energy_model/building_energy_model.hpp"

namespace {

testing::TestOutcome run_building_energy_model() {
    building_energy_model::init();
    auto s = building_energy_model::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    building_energy_model::shutdown();
    return testing::ok();
}

const int _reg_building_energy_model = testing::register_test(
    "building_energy_model",
    "1045_building_energy_model: stub status check",
    &run_building_energy_model);

}
