#include "test_runner.hpp"
#include "../modules/985_alloy_database_browser/alloy_database_browser.hpp"

namespace {

testing::TestOutcome run_alloy_database_browser() {
    alloy_database_browser::init();
    auto s = alloy_database_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    alloy_database_browser::shutdown();
    return testing::ok();
}

const int _reg_alloy_database_browser = testing::register_test(
    "alloy_database_browser",
    "985_alloy_database_browser: stub status check",
    &run_alloy_database_browser);

}
