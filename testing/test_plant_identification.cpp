// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1599_plant_identification/plant_identification.hpp"

namespace {

testing::TestOutcome run_plant_identification() {
    plant_identification::init();
    auto s = plant_identification::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plant_identification::shutdown();
    return testing::ok();
}

const int _reg_plant_identification = testing::register_test(
    "plant_identification",
    "1599_plant_identification: stub status check",
    &run_plant_identification);

}
