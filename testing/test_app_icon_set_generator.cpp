// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/160_app_icon_set_generator/app_icon_set_generator.hpp"

namespace {

testing::TestOutcome run_app_icon_set_generator() {
    app_icon_set_generator::init();
    auto s = app_icon_set_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    app_icon_set_generator::shutdown();
    return testing::ok();
}

const int _reg_app_icon_set_generator = testing::register_test(
    "app_icon_set_generator",
    "160_app_icon_set_generator: stub status check",
    &run_app_icon_set_generator);

}
