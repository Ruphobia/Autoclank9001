// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/251_embedded_sqlite_duckdb_workbench/embedded_sqlite_duckdb_workbench.hpp"

namespace {

testing::TestOutcome run_embedded_sqlite_duckdb_workbench() {
    embedded_sqlite_duckdb_workbench::init();
    auto s = embedded_sqlite_duckdb_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    embedded_sqlite_duckdb_workbench::shutdown();
    return testing::ok();
}

const int _reg_embedded_sqlite_duckdb_workbench = testing::register_test(
    "embedded_sqlite_duckdb_workbench",
    "251_embedded_sqlite_duckdb_workbench: stub status check",
    &run_embedded_sqlite_duckdb_workbench);

}
