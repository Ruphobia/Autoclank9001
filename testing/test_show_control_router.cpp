// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1928_show_control_router/show_control_router.hpp"

namespace {

testing::TestOutcome run_show_control_router() {
    show_control_router::init();
    auto s = show_control_router::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    show_control_router::shutdown();
    return testing::ok();
}

const int _reg_show_control_router = testing::register_test(
    "show_control_router",
    "1928_show_control_router: stub status check",
    &run_show_control_router);

}
