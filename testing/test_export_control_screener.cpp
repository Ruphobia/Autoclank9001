// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/596_export_control_screener/export_control_screener.hpp"

namespace {

testing::TestOutcome run_export_control_screener() {
    export_control_screener::init();
    auto s = export_control_screener::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    export_control_screener::shutdown();
    return testing::ok();
}

const int _reg_export_control_screener = testing::register_test(
    "export_control_screener",
    "596_export_control_screener: stub status check",
    &run_export_control_screener);

}
