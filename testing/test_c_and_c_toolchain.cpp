// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/105_c_and_c_toolchain/c_and_c_toolchain.hpp"

namespace {

testing::TestOutcome run_c_and_c_toolchain() {
    c_and_c_toolchain::init();
    auto s = c_and_c_toolchain::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    c_and_c_toolchain::shutdown();
    return testing::ok();
}

const int _reg_c_and_c_toolchain = testing::register_test(
    "c_and_c_toolchain",
    "105_c_and_c_toolchain: stub status check",
    &run_c_and_c_toolchain);

}
