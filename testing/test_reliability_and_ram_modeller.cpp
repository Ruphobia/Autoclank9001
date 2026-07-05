// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1123_reliability_and_ram_modeller/reliability_and_ram_modeller.hpp"

namespace {

testing::TestOutcome run_reliability_and_ram_modeller() {
    reliability_and_ram_modeller::init();
    auto s = reliability_and_ram_modeller::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reliability_and_ram_modeller::shutdown();
    return testing::ok();
}

const int _reg_reliability_and_ram_modeller = testing::register_test(
    "reliability_and_ram_modeller",
    "1123_reliability_and_ram_modeller: stub status check",
    &run_reliability_and_ram_modeller);

}
