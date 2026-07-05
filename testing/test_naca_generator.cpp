// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1148_naca_generator/naca_generator.hpp"

namespace {

testing::TestOutcome run_naca_generator() {
    naca_generator::init();
    auto s = naca_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    naca_generator::shutdown();
    return testing::ok();
}

const int _reg_naca_generator = testing::register_test(
    "naca_generator",
    "1148_naca_generator: stub status check",
    &run_naca_generator);

}
