// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/738_ritual_database/ritual_database.hpp"

namespace {

testing::TestOutcome run_ritual_database() {
    ritual_database::init();
    auto s = ritual_database::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ritual_database::shutdown();
    return testing::ok();
}

const int _reg_ritual_database = testing::register_test(
    "ritual_database",
    "738_ritual_database: stub status check",
    &run_ritual_database);

}
