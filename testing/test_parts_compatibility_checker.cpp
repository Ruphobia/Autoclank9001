// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1240_parts_compatibility_checker/parts_compatibility_checker.hpp"

namespace {

testing::TestOutcome run_parts_compatibility_checker() {
    parts_compatibility_checker::init();
    auto s = parts_compatibility_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    parts_compatibility_checker::shutdown();
    return testing::ok();
}

const int _reg_parts_compatibility_checker = testing::register_test(
    "parts_compatibility_checker",
    "1240_parts_compatibility_checker: stub status check",
    &run_parts_compatibility_checker);

}
