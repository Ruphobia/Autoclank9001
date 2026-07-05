// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/320_passthrough_compositor/passthrough_compositor.hpp"

namespace {

testing::TestOutcome run_passthrough_compositor() {
    passthrough_compositor::init();
    auto s = passthrough_compositor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    passthrough_compositor::shutdown();
    return testing::ok();
}

const int _reg_passthrough_compositor = testing::register_test(
    "passthrough_compositor",
    "320_passthrough_compositor: stub status check",
    &run_passthrough_compositor);

}
