// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2013_species_care_database/species_care_database.hpp"

namespace {

testing::TestOutcome run_species_care_database() {
    species_care_database::init();
    auto s = species_care_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    species_care_database::shutdown();
    return testing::ok();
}

const int _reg_species_care_database = testing::register_test(
    "species_care_database",
    "2013_species_care_database: stub status check",
    &run_species_care_database);

}
