// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1023_valve_cv_and_kv_sizer/valve_cv_and_kv_sizer.hpp"

namespace {

testing::TestOutcome run_valve_cv_and_kv_sizer() {
    valve_cv_and_kv_sizer::init();
    auto s = valve_cv_and_kv_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    valve_cv_and_kv_sizer::shutdown();
    return testing::ok();
}

const int _reg_valve_cv_and_kv_sizer = testing::register_test(
    "valve_cv_and_kv_sizer",
    "1023_valve_cv_and_kv_sizer: stub status check",
    &run_valve_cv_and_kv_sizer);

}
