// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/186_swo_itm_rtt_console/swo_itm_rtt_console.hpp"

namespace {

testing::TestOutcome run_swo_itm_rtt_console() {
    swo_itm_rtt_console::init();
    auto s = swo_itm_rtt_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    swo_itm_rtt_console::shutdown();
    return testing::ok();
}

const int _reg_swo_itm_rtt_console = testing::register_test(
    "swo_itm_rtt_console",
    "186_swo_itm_rtt_console: stub status check",
    &run_swo_itm_rtt_console);

}
