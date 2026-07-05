// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/706_declension_and_conjugation_tables/declension_and_conjugation_tables.hpp"

namespace {

testing::TestOutcome run_declension_and_conjugation_tables() {
    declension_and_conjugation_tables::init();
    auto s = declension_and_conjugation_tables::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    declension_and_conjugation_tables::shutdown();
    return testing::ok();
}

const int _reg_declension_and_conjugation_tables = testing::register_test(
    "declension_and_conjugation_tables",
    "706_declension_and_conjugation_tables: stub status check",
    &run_declension_and_conjugation_tables);

}
