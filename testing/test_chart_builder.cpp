// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/430_chart_builder/chart_builder.hpp"

namespace {

testing::TestOutcome run_chart_builder() {
    chart_builder::init();
    auto s = chart_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    chart_builder::shutdown();
    return testing::ok();
}

const int _reg_chart_builder = testing::register_test(
    "chart_builder",
    "430_chart_builder: stub status check",
    &run_chart_builder);

}
