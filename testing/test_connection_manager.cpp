// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/224_connection_manager/connection_manager.hpp"

namespace {

testing::TestOutcome run_connection_manager() {
    connection_manager::init();
    auto s = connection_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    connection_manager::shutdown();
    return testing::ok();
}

const int _reg_connection_manager = testing::register_test(
    "connection_manager",
    "224_connection_manager: stub status check",
    &run_connection_manager);

}
