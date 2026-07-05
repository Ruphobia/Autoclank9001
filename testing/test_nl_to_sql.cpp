// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/252_nl_to_sql/nl_to_sql.hpp"

namespace {

testing::TestOutcome run_nl_to_sql() {
    nl_to_sql::init();
    auto s = nl_to_sql::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nl_to_sql::shutdown();
    return testing::ok();
}

const int _reg_nl_to_sql = testing::register_test(
    "nl_to_sql",
    "252_nl_to_sql: stub status check",
    &run_nl_to_sql);

}
