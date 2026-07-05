// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/228_schema_diff/schema_diff.hpp"

namespace {

testing::TestOutcome run_schema_diff() {
    schema_diff::init();
    auto s = schema_diff::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    schema_diff::shutdown();
    return testing::ok();
}

const int _reg_schema_diff = testing::register_test(
    "schema_diff",
    "228_schema_diff: stub status check",
    &run_schema_diff);

}
