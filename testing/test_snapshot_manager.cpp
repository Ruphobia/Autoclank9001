// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/265_snapshot_manager/snapshot_manager.hpp"

namespace {

testing::TestOutcome run_snapshot_manager() {
    snapshot_manager::init();
    auto s = snapshot_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    snapshot_manager::shutdown();
    return testing::ok();
}

const int _reg_snapshot_manager = testing::register_test(
    "snapshot_manager",
    "265_snapshot_manager: stub status check",
    &run_snapshot_manager);

}
