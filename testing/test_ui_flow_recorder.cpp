// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/172_ui_flow_recorder/ui_flow_recorder.hpp"

namespace {

testing::TestOutcome run_ui_flow_recorder() {
    ui_flow_recorder::init();
    auto s = ui_flow_recorder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ui_flow_recorder::shutdown();
    return testing::ok();
}

const int _reg_ui_flow_recorder = testing::register_test(
    "ui_flow_recorder",
    "172_ui_flow_recorder: stub status check",
    &run_ui_flow_recorder);

}
