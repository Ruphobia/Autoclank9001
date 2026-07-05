// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/699_regesta_builder/regesta_builder.hpp"

namespace {

testing::TestOutcome run_regesta_builder() {
    regesta_builder::init();
    auto s = regesta_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    regesta_builder::shutdown();
    return testing::ok();
}

const int _reg_regesta_builder = testing::register_test(
    "regesta_builder",
    "699_regesta_builder: stub status check",
    &run_regesta_builder);

}
