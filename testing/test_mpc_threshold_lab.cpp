// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/396_mpc_threshold_lab/mpc_threshold_lab.hpp"

namespace {

testing::TestOutcome run_mpc_threshold_lab() {
    mpc_threshold_lab::init();
    auto s = mpc_threshold_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mpc_threshold_lab::shutdown();
    return testing::ok();
}

const int _reg_mpc_threshold_lab = testing::register_test(
    "mpc_threshold_lab",
    "396_mpc_threshold_lab: stub status check",
    &run_mpc_threshold_lab);

}
