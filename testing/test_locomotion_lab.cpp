// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/319_locomotion_lab/locomotion_lab.hpp"

namespace {

testing::TestOutcome run_locomotion_lab() {
    locomotion_lab::init();
    auto s = locomotion_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    locomotion_lab::shutdown();
    return testing::ok();
}

const int _reg_locomotion_lab = testing::register_test(
    "locomotion_lab",
    "319_locomotion_lab: stub status check",
    &run_locomotion_lab);

}
