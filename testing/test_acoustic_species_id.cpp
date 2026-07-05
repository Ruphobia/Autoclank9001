// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1500_acoustic_species_id/acoustic_species_id.hpp"

namespace {

testing::TestOutcome run_acoustic_species_id() {
    acoustic_species_id::init();
    auto s = acoustic_species_id::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    acoustic_species_id::shutdown();
    return testing::ok();
}

const int _reg_acoustic_species_id = testing::register_test(
    "acoustic_species_id",
    "1500_acoustic_species_id: stub status check",
    &run_acoustic_species_id);

}
