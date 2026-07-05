// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1347_sequence_viewer/sequence_viewer.hpp"

namespace {

testing::TestOutcome run_sequence_viewer() {
    sequence_viewer::init();
    auto s = sequence_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sequence_viewer::shutdown();
    return testing::ok();
}

const int _reg_sequence_viewer = testing::register_test(
    "sequence_viewer",
    "1347_sequence_viewer: stub status check",
    &run_sequence_viewer);

}
