// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/204_fuzzer_and_property_tester/fuzzer_and_property_tester.hpp"

namespace {

testing::TestOutcome run_fuzzer_and_property_tester() {
    fuzzer_and_property_tester::init();
    auto s = fuzzer_and_property_tester::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fuzzer_and_property_tester::shutdown();
    return testing::ok();
}

const int _reg_fuzzer_and_property_tester = testing::register_test(
    "fuzzer_and_property_tester",
    "204_fuzzer_and_property_tester: stub status check",
    &run_fuzzer_and_property_tester);

}
