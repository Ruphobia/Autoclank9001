// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/680_activation_explorer/activation_explorer.hpp"

namespace {

testing::TestOutcome run_activation_explorer() {
    activation_explorer::init();
    auto s = activation_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    activation_explorer::shutdown();
    return testing::ok();
}

const int _reg_activation_explorer = testing::register_test(
    "activation_explorer",
    "680_activation_explorer: stub status check",
    &run_activation_explorer);

}
