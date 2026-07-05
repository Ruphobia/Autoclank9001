// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/148_api_schema_explorer/api_schema_explorer.hpp"

namespace {

testing::TestOutcome run_api_schema_explorer() {
    api_schema_explorer::init();
    auto s = api_schema_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    api_schema_explorer::shutdown();
    return testing::ok();
}

const int _reg_api_schema_explorer = testing::register_test(
    "api_schema_explorer",
    "148_api_schema_explorer: stub status check",
    &run_api_schema_explorer);

}
