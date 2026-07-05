// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/201_hal_and_driver_generator/hal_and_driver_generator.hpp"

namespace {

testing::TestOutcome run_hal_and_driver_generator() {
    hal_and_driver_generator::init();
    auto s = hal_and_driver_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hal_and_driver_generator::shutdown();
    return testing::ok();
}

const int _reg_hal_and_driver_generator = testing::register_test(
    "hal_and_driver_generator",
    "201_hal_and_driver_generator: stub status check",
    &run_hal_and_driver_generator);

}
