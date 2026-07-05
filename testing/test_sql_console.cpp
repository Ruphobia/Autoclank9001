#include "test_runner.hpp"
#include "../modules/223_sql_console/sql_console.hpp"

namespace {

testing::TestOutcome run_sql_console() {
    sql_console::init();
    auto s = sql_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sql_console::shutdown();
    return testing::ok();
}

const int _reg_sql_console = testing::register_test(
    "sql_console",
    "223_sql_console: stub status check",
    &run_sql_console);

}
