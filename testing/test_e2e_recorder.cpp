// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/130_e2e_recorder/e2e_recorder.hpp"

namespace {

testing::TestOutcome run_e2e_recorder() {
    e2e_recorder::init();
    auto s = e2e_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    e2e_recorder::shutdown();
    return testing::ok();
}

const int _reg_e2e_recorder = testing::register_test(
    "e2e_recorder",
    "130_e2e_recorder: stub status check",
    &run_e2e_recorder);

}
