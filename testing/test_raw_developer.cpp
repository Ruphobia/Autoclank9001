// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1635_raw_developer/raw_developer.hpp"

namespace {

testing::TestOutcome run_raw_developer() {
    raw_developer::init();
    auto s = raw_developer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    raw_developer::shutdown();
    return testing::ok();
}

const int _reg_raw_developer = testing::register_test(
    "raw_developer",
    "1635_raw_developer: stub status check",
    &run_raw_developer);

}
