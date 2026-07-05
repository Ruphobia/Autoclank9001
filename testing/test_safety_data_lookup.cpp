// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1309_safety_data_lookup/safety_data_lookup.hpp"

namespace {

testing::TestOutcome run_safety_data_lookup() {
    safety_data_lookup::init();
    auto s = safety_data_lookup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    safety_data_lookup::shutdown();
    return testing::ok();
}

const int _reg_safety_data_lookup = testing::register_test(
    "safety_data_lookup",
    "1309_safety_data_lookup: stub status check",
    &run_safety_data_lookup);

}
