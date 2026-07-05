// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1277_mppt_tracker_modeler/mppt_tracker_modeler.hpp"

namespace {

testing::TestOutcome run_mppt_tracker_modeler() {
    mppt_tracker_modeler::init();
    auto s = mppt_tracker_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mppt_tracker_modeler::shutdown();
    return testing::ok();
}

const int _reg_mppt_tracker_modeler = testing::register_test(
    "mppt_tracker_modeler",
    "1277_mppt_tracker_modeler: stub status check",
    &run_mppt_tracker_modeler);

}
