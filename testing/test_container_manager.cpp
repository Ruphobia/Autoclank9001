// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/266_container_manager/container_manager.hpp"

namespace {

testing::TestOutcome run_container_manager() {
    container_manager::init();
    auto s = container_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    container_manager::shutdown();
    return testing::ok();
}

const int _reg_container_manager = testing::register_test(
    "container_manager",
    "266_container_manager: stub status check",
    &run_container_manager);

}
