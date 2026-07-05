// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/329_xr_input_recorder/xr_input_recorder.hpp"

namespace {

testing::TestOutcome run_xr_input_recorder() {
    xr_input_recorder::init();
    auto s = xr_input_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    xr_input_recorder::shutdown();
    return testing::ok();
}

const int _reg_xr_input_recorder = testing::register_test(
    "xr_input_recorder",
    "329_xr_input_recorder: stub status check",
    &run_xr_input_recorder);

}
