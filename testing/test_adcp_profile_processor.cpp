// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1553_adcp_profile_processor/adcp_profile_processor.hpp"

namespace {

testing::TestOutcome run_adcp_profile_processor() {
    adcp_profile_processor::init();
    auto s = adcp_profile_processor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    adcp_profile_processor::shutdown();
    return testing::ok();
}

const int _reg_adcp_profile_processor = testing::register_test(
    "adcp_profile_processor",
    "1553_adcp_profile_processor: stub status check",
    &run_adcp_profile_processor);

}
