// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/899_statistics_workbench/statistics_workbench.hpp"

namespace {

testing::TestOutcome run_statistics_workbench() {
    statistics_workbench::init();
    auto s = statistics_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    statistics_workbench::shutdown();
    return testing::ok();
}

const int _reg_statistics_workbench = testing::register_test(
    "statistics_workbench",
    "899_statistics_workbench: stub status check",
    &run_statistics_workbench);

}
