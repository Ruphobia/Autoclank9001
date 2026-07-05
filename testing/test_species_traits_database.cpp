// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1576_species_traits_database/species_traits_database.hpp"

namespace {

testing::TestOutcome run_species_traits_database() {
    species_traits_database::init();
    auto s = species_traits_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    species_traits_database::shutdown();
    return testing::ok();
}

const int _reg_species_traits_database = testing::register_test(
    "species_traits_database",
    "1576_species_traits_database: stub status check",
    &run_species_traits_database);

}
