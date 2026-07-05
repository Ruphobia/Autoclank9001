// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/660_time_series_lab/time_series_lab.hpp"

namespace {

testing::TestOutcome run_time_series_lab() {
    time_series_lab::init();
    auto s = time_series_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    time_series_lab::shutdown();
    return testing::ok();
}

const int _reg_time_series_lab = testing::register_test(
    "time_series_lab",
    "660_time_series_lab: stub status check",
    &run_time_series_lab);

}
