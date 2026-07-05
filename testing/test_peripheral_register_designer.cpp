// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/180_peripheral_register_designer/peripheral_register_designer.hpp"

namespace {

testing::TestOutcome run_peripheral_register_designer() {
    peripheral_register_designer::init();
    auto s = peripheral_register_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    peripheral_register_designer::shutdown();
    return testing::ok();
}

const int _reg_peripheral_register_designer = testing::register_test(
    "peripheral_register_designer",
    "180_peripheral_register_designer: stub status check",
    &run_peripheral_register_designer);

}
