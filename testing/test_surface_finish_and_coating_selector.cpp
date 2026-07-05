// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1000_surface_finish_and_coating_selector/surface_finish_and_coating_selector.hpp"

namespace {

testing::TestOutcome run_surface_finish_and_coating_selector() {
    surface_finish_and_coating_selector::init();
    auto s = surface_finish_and_coating_selector::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    surface_finish_and_coating_selector::shutdown();
    return testing::ok();
}

const int _reg_surface_finish_and_coating_selector = testing::register_test(
    "surface_finish_and_coating_selector",
    "1000_surface_finish_and_coating_selector: stub status check",
    &run_surface_finish_and_coating_selector);

}
