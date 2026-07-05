// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/526_worked_example_generator/worked_example_generator.hpp"

namespace {

testing::TestOutcome run_worked_example_generator() {
    worked_example_generator::init();
    auto s = worked_example_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    worked_example_generator::shutdown();
    return testing::ok();
}

const int _reg_worked_example_generator = testing::register_test(
    "worked_example_generator",
    "526_worked_example_generator: stub status check",
    &run_worked_example_generator);

}
