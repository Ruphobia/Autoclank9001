// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/772_mythography_database/mythography_database.hpp"

namespace {

testing::TestOutcome run_mythography_database() {
    mythography_database::init();
    auto s = mythography_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mythography_database::shutdown();
    return testing::ok();
}

const int _reg_mythography_database = testing::register_test(
    "mythography_database",
    "772_mythography_database: stub status check",
    &run_mythography_database);

}
