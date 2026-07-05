// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/542_life_contingencies_pricer/life_contingencies_pricer.hpp"

namespace {

testing::TestOutcome run_life_contingencies_pricer() {
    life_contingencies_pricer::init();
    auto s = life_contingencies_pricer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    life_contingencies_pricer::shutdown();
    return testing::ok();
}

const int _reg_life_contingencies_pricer = testing::register_test(
    "life_contingencies_pricer",
    "542_life_contingencies_pricer: stub status check",
    &run_life_contingencies_pricer);

}
