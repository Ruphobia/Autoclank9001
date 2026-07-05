// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/780_cosmogony_comparator/cosmogony_comparator.hpp"

namespace {

testing::TestOutcome run_cosmogony_comparator() {
    cosmogony_comparator::init();
    auto s = cosmogony_comparator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cosmogony_comparator::shutdown();
    return testing::ok();
}

const int _reg_cosmogony_comparator = testing::register_test(
    "cosmogony_comparator",
    "780_cosmogony_comparator: stub status check",
    &run_cosmogony_comparator);

}
