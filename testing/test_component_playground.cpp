// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/118_component_playground/component_playground.hpp"

namespace {

testing::TestOutcome run_component_playground() {
    component_playground::init();
    auto s = component_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    component_playground::shutdown();
    return testing::ok();
}

const int _reg_component_playground = testing::register_test(
    "component_playground",
    "118_component_playground: stub status check",
    &run_component_playground);

}
