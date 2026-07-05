// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/589_dmca_toolkit/dmca_toolkit.hpp"

namespace {

testing::TestOutcome run_dmca_toolkit() {
    dmca_toolkit::init();
    auto s = dmca_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dmca_toolkit::shutdown();
    return testing::ok();
}

const int _reg_dmca_toolkit = testing::register_test(
    "dmca_toolkit",
    "589_dmca_toolkit: stub status check",
    &run_dmca_toolkit);

}
