// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/487_facility_location_optimizer/facility_location_optimizer.hpp"

namespace {

testing::TestOutcome run_facility_location_optimizer() {
    facility_location_optimizer::init();
    auto s = facility_location_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    facility_location_optimizer::shutdown();
    return testing::ok();
}

const int _reg_facility_location_optimizer = testing::register_test(
    "facility_location_optimizer",
    "487_facility_location_optimizer: stub status check",
    &run_facility_location_optimizer);

}
