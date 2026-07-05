// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/253_seed_data_generator/seed_data_generator.hpp"

namespace {

testing::TestOutcome run_seed_data_generator() {
    seed_data_generator::init();
    auto s = seed_data_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    seed_data_generator::shutdown();
    return testing::ok();
}

const int _reg_seed_data_generator = testing::register_test(
    "seed_data_generator",
    "253_seed_data_generator: stub status check",
    &run_seed_data_generator);

}
