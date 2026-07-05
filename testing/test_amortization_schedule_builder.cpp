// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/532_amortization_schedule_builder/amortization_schedule_builder.hpp"

namespace {

testing::TestOutcome run_amortization_schedule_builder() {
    amortization_schedule_builder::init();
    auto s = amortization_schedule_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    amortization_schedule_builder::shutdown();
    return testing::ok();
}

const int _reg_amortization_schedule_builder = testing::register_test(
    "amortization_schedule_builder",
    "532_amortization_schedule_builder: stub status check",
    &run_amortization_schedule_builder);

}
