// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1290_soiling_snow_degradation_model/soiling_snow_degradation_model.hpp"

namespace {

testing::TestOutcome run_soiling_snow_degradation_model() {
    soiling_snow_degradation_model::init();
    auto s = soiling_snow_degradation_model::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    soiling_snow_degradation_model::shutdown();
    return testing::ok();
}

const int _reg_soiling_snow_degradation_model = testing::register_test(
    "soiling_snow_degradation_model",
    "1290_soiling_snow_degradation_model: stub status check",
    &run_soiling_snow_degradation_model);

}
