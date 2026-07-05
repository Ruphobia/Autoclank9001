#include "test_runner.hpp"
#include "../modules/225_schema_browser/schema_browser.hpp"

namespace {

testing::TestOutcome run_schema_browser() {
    schema_browser::init();
    auto s = schema_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    schema_browser::shutdown();
    return testing::ok();
}

const int _reg_schema_browser = testing::register_test(
    "schema_browser",
    "225_schema_browser: stub status check",
    &run_schema_browser);

}
