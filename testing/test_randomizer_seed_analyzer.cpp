// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1980_randomizer_seed_analyzer/randomizer_seed_analyzer.hpp"

namespace {

testing::TestOutcome run_randomizer_seed_analyzer() {
    randomizer_seed_analyzer::init();
    auto s = randomizer_seed_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    randomizer_seed_analyzer::shutdown();
    return testing::ok();
}

const int _reg_randomizer_seed_analyzer = testing::register_test(
    "randomizer_seed_analyzer",
    "1980_randomizer_seed_analyzer: stub status check",
    &run_randomizer_seed_analyzer);

}
