// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/918_optimization_studio/optimization_studio.hpp"

namespace {

testing::TestOutcome run_optimization_studio() {
    optimization_studio::init();
    auto s = optimization_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    optimization_studio::shutdown();
    return testing::ok();
}

const int _reg_optimization_studio = testing::register_test(
    "optimization_studio",
    "918_optimization_studio: stub status check",
    &run_optimization_studio);

}
