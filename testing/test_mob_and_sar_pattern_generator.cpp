// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2109_mob_and_sar_pattern_generator/mob_and_sar_pattern_generator.hpp"

namespace {

testing::TestOutcome run_mob_and_sar_pattern_generator() {
    mob_and_sar_pattern_generator::init();
    auto s = mob_and_sar_pattern_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mob_and_sar_pattern_generator::shutdown();
    return testing::ok();
}

const int _reg_mob_and_sar_pattern_generator = testing::register_test(
    "mob_and_sar_pattern_generator",
    "2109_mob_and_sar_pattern_generator: stub status check",
    &run_mob_and_sar_pattern_generator);

}
