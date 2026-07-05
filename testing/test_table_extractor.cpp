// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/622_table_extractor/table_extractor.hpp"

namespace {

testing::TestOutcome run_table_extractor() {
    table_extractor::init();
    auto s = table_extractor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    table_extractor::shutdown();
    return testing::ok();
}

const int _reg_table_extractor = testing::register_test(
    "table_extractor",
    "622_table_extractor: stub status check",
    &run_table_extractor);

}
