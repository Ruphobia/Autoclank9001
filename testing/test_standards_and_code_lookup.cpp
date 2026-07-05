// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1118_standards_and_code_lookup/standards_and_code_lookup.hpp"

namespace {

testing::TestOutcome run_standards_and_code_lookup() {
    standards_and_code_lookup::init();
    auto s = standards_and_code_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    standards_and_code_lookup::shutdown();
    return testing::ok();
}

const int _reg_standards_and_code_lookup = testing::register_test(
    "standards_and_code_lookup",
    "1118_standards_and_code_lookup: stub status check",
    &run_standards_and_code_lookup);

}
