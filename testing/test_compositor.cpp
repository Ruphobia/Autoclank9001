// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1650_compositor/compositor.hpp"

namespace {

testing::TestOutcome run_compositor() {
    compositor::init();
    auto s = compositor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    compositor::shutdown();
    return testing::ok();
}

const int _reg_compositor = testing::register_test(
    "compositor",
    "1650_compositor: stub status check",
    &run_compositor);

}
