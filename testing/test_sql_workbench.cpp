#include "test_runner.hpp"
#include "../modules/099_sql_workbench/sql_workbench.hpp"

namespace {

testing::TestOutcome run_sql_workbench() {
    sql_workbench::init();
    auto s = sql_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sql_workbench::shutdown();
    return testing::ok();
}

const int _reg_sql_workbench = testing::register_test(
    "sql_workbench",
    "099_sql_workbench: stub status check",
    &run_sql_workbench);

}
