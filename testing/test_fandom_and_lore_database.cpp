// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2203_fandom_and_lore_database/fandom_and_lore_database.hpp"

namespace {

testing::TestOutcome run_fandom_and_lore_database() {
    fandom_and_lore_database::init();
    auto s = fandom_and_lore_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fandom_and_lore_database::shutdown();
    return testing::ok();
}

const int _reg_fandom_and_lore_database = testing::register_test(
    "fandom_and_lore_database",
    "2203_fandom_and_lore_database: stub status check",
    &run_fandom_and_lore_database);

}
