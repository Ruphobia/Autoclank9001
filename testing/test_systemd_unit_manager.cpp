// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/257_systemd_unit_manager/systemd_unit_manager.hpp"

namespace {

testing::TestOutcome run_systemd_unit_manager() {
    systemd_unit_manager::init();
    auto s = systemd_unit_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    systemd_unit_manager::shutdown();
    return testing::ok();
}

const int _reg_systemd_unit_manager = testing::register_test(
    "systemd_unit_manager",
    "257_systemd_unit_manager: stub status check",
    &run_systemd_unit_manager);

}
