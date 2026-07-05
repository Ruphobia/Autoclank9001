// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2079_training_load_pmc/training_load_pmc.hpp"

namespace {

testing::TestOutcome run_training_load_pmc() {
    training_load_pmc::init();
    auto s = training_load_pmc::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    training_load_pmc::shutdown();
    return testing::ok();
}

const int _reg_training_load_pmc = testing::register_test(
    "training_load_pmc",
    "2079_training_load_pmc: stub status check",
    &run_training_load_pmc);

}
