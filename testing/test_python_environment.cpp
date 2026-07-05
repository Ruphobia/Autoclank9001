// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/106_python_environment/python_environment.hpp"

namespace {

testing::TestOutcome run_python_environment() {
    python_environment::init();
    auto s = python_environment::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    python_environment::shutdown();
    return testing::ok();
}

const int _reg_python_environment = testing::register_test(
    "python_environment",
    "106_python_environment: stub status check",
    &run_python_environment);

}
