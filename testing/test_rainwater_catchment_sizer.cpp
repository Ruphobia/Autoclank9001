// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2115_rainwater_catchment_sizer/rainwater_catchment_sizer.hpp"

namespace {

testing::TestOutcome run_rainwater_catchment_sizer() {
    rainwater_catchment_sizer::init();
    auto s = rainwater_catchment_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rainwater_catchment_sizer::shutdown();
    return testing::ok();
}

const int _reg_rainwater_catchment_sizer = testing::register_test(
    "rainwater_catchment_sizer",
    "2115_rainwater_catchment_sizer: stub status check",
    &run_rainwater_catchment_sizer);

}
