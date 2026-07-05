// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/263_filesystem_and_mount_manager/filesystem_and_mount_manager.hpp"

namespace {

testing::TestOutcome run_filesystem_and_mount_manager() {
    filesystem_and_mount_manager::init();
    auto s = filesystem_and_mount_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    filesystem_and_mount_manager::shutdown();
    return testing::ok();
}

const int _reg_filesystem_and_mount_manager = testing::register_test(
    "filesystem_and_mount_manager",
    "263_filesystem_and_mount_manager: stub status check",
    &run_filesystem_and_mount_manager);

}
