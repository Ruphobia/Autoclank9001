// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/076_standing_principle_small_specialist_integration/standing_principle_small_specialist_integration.hpp"

namespace {

testing::TestOutcome run_standing_principle_small_specialist_integration() {
    standing_principle_small_specialist_integration::init();
    auto s = standing_principle_small_specialist_integration::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    standing_principle_small_specialist_integration::shutdown();
    return testing::ok();
}

const int _reg_standing_principle_small_specialist_integration = testing::register_test(
    "standing_principle_small_specialist_integration",
    "076_standing_principle_small_specialist_integration: stub status check",
    &run_standing_principle_small_specialist_integration);

}
