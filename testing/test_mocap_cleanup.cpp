// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1686_mocap_cleanup/mocap_cleanup.hpp"

namespace {

testing::TestOutcome run_mocap_cleanup() {
    mocap_cleanup::init();
    auto s = mocap_cleanup::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mocap_cleanup::shutdown();
    return testing::ok();
}

const int _reg_mocap_cleanup = testing::register_test(
    "mocap_cleanup",
    "1686_mocap_cleanup: stub status check",
    &run_mocap_cleanup);

}
