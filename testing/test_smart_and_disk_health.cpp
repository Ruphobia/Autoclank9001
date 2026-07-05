// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/262_smart_and_disk_health/smart_and_disk_health.hpp"

namespace {

testing::TestOutcome run_smart_and_disk_health() {
    smart_and_disk_health::init();
    auto s = smart_and_disk_health::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    smart_and_disk_health::shutdown();
    return testing::ok();
}

const int _reg_smart_and_disk_health = testing::register_test(
    "smart_and_disk_health",
    "262_smart_and_disk_health: stub status check",
    &run_smart_and_disk_health);

}
