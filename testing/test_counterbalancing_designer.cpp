// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1420_counterbalancing_designer/counterbalancing_designer.hpp"

namespace {

testing::TestOutcome run_counterbalancing_designer() {
    counterbalancing_designer::init();
    auto s = counterbalancing_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    counterbalancing_designer::shutdown();
    return testing::ok();
}

const int _reg_counterbalancing_designer = testing::register_test(
    "counterbalancing_designer",
    "1420_counterbalancing_designer: stub status check",
    &run_counterbalancing_designer);

}
