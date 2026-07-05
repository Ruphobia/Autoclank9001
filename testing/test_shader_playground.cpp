// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/294_shader_playground/shader_playground.hpp"

namespace {

testing::TestOutcome run_shader_playground() {
    shader_playground::init();
    auto s = shader_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    shader_playground::shutdown();
    return testing::ok();
}

const int _reg_shader_playground = testing::register_test(
    "shader_playground",
    "294_shader_playground: stub status check",
    &run_shader_playground);

}
