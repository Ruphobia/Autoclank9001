// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1239_frame_geometry_comparator/frame_geometry_comparator.hpp"

namespace {

testing::TestOutcome run_frame_geometry_comparator() {
    frame_geometry_comparator::init();
    auto s = frame_geometry_comparator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    frame_geometry_comparator::shutdown();
    return testing::ok();
}

const int _reg_frame_geometry_comparator = testing::register_test(
    "frame_geometry_comparator",
    "1239_frame_geometry_comparator: stub status check",
    &run_frame_geometry_comparator);

}
