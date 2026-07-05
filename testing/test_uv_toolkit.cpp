// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1693_uv_toolkit/uv_toolkit.hpp"

namespace {

testing::TestOutcome run_uv_toolkit() {
    uv_toolkit::init();
    auto s = uv_toolkit::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    uv_toolkit::shutdown();
    return testing::ok();
}

const int _reg_uv_toolkit = testing::register_test(
    "uv_toolkit",
    "1693_uv_toolkit: stub status check",
    &run_uv_toolkit);

}
