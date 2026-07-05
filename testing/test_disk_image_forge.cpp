// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/399_disk_image_forge/disk_image_forge.hpp"

namespace {

testing::TestOutcome run_disk_image_forge() {
    disk_image_forge::init();
    auto s = disk_image_forge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    disk_image_forge::shutdown();
    return testing::ok();
}

const int _reg_disk_image_forge = testing::register_test(
    "disk_image_forge",
    "399_disk_image_forge: stub status check",
    &run_disk_image_forge);

}
