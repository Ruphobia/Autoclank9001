// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/152_load_tester/load_tester.hpp"

namespace {

testing::TestOutcome run_load_tester() {
    load_tester::init();
    auto s = load_tester::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    load_tester::shutdown();
    return testing::ok();
}

const int _reg_load_tester = testing::register_test(
    "load_tester",
    "152_load_tester: stub status check",
    &run_load_tester);

}
