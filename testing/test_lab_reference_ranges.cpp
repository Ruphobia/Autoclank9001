// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1409_lab_reference_ranges/lab_reference_ranges.hpp"

namespace {

testing::TestOutcome run_lab_reference_ranges() {
    lab_reference_ranges::init();
    auto s = lab_reference_ranges::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lab_reference_ranges::shutdown();
    return testing::ok();
}

const int _reg_lab_reference_ranges = testing::register_test(
    "lab_reference_ranges",
    "1409_lab_reference_ranges: stub status check",
    &run_lab_reference_ranges);

}
