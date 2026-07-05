// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1164_standard_atmosphere/standard_atmosphere.hpp"

namespace {

testing::TestOutcome run_standard_atmosphere() {
    standard_atmosphere::init();
    auto s = standard_atmosphere::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    standard_atmosphere::shutdown();
    return testing::ok();
}

const int _reg_standard_atmosphere = testing::register_test(
    "standard_atmosphere",
    "1164_standard_atmosphere: stub status check",
    &run_standard_atmosphere);

}
