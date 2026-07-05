// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/357_disk_image_browser/disk_image_browser.hpp"

namespace {

testing::TestOutcome run_disk_image_browser() {
    disk_image_browser::init();
    auto s = disk_image_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    disk_image_browser::shutdown();
    return testing::ok();
}

const int _reg_disk_image_browser = testing::register_test(
    "disk_image_browser",
    "357_disk_image_browser: stub status check",
    &run_disk_image_browser);

}
