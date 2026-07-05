// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2133_segment_triangulator/segment_triangulator.hpp"

namespace {

testing::TestOutcome run_segment_triangulator() {
    segment_triangulator::init();
    auto s = segment_triangulator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    segment_triangulator::shutdown();
    return testing::ok();
}

const int _reg_segment_triangulator = testing::register_test(
    "segment_triangulator",
    "2133_segment_triangulator: stub status check",
    &run_segment_triangulator);

}
