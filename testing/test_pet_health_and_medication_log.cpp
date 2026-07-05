// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2016_pet_health_and_medication_log/pet_health_and_medication_log.hpp"

namespace {

testing::TestOutcome run_pet_health_and_medication_log() {
    pet_health_and_medication_log::init();
    auto s = pet_health_and_medication_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pet_health_and_medication_log::shutdown();
    return testing::ok();
}

const int _reg_pet_health_and_medication_log = testing::register_test(
    "pet_health_and_medication_log",
    "2016_pet_health_and_medication_log: stub status check",
    &run_pet_health_and_medication_log);

}
