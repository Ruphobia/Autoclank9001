// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1690_render_farm_dispatcher/render_farm_dispatcher.hpp"

namespace {

testing::TestOutcome run_render_farm_dispatcher() {
    render_farm_dispatcher::init();
    auto s = render_farm_dispatcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    render_farm_dispatcher::shutdown();
    return testing::ok();
}

const int _reg_render_farm_dispatcher = testing::register_test(
    "render_farm_dispatcher",
    "1690_render_farm_dispatcher: stub status check",
    &run_render_farm_dispatcher);

}
