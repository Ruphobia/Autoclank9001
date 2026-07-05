// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1018_insulation_thickness_optimizer/insulation_thickness_optimizer.hpp"

namespace {

testing::TestOutcome run_insulation_thickness_optimizer() {
    insulation_thickness_optimizer::init();
    auto s = insulation_thickness_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    insulation_thickness_optimizer::shutdown();
    return testing::ok();
}

const int _reg_insulation_thickness_optimizer = testing::register_test(
    "insulation_thickness_optimizer",
    "1018_insulation_thickness_optimizer: stub status check",
    &run_insulation_thickness_optimizer);

}
