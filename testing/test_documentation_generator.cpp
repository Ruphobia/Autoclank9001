// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/102_documentation_generator/documentation_generator.hpp"

namespace {

testing::TestOutcome run_documentation_generator() {
    documentation_generator::init();
    auto s = documentation_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    documentation_generator::shutdown();
    return testing::ok();
}

const int _reg_documentation_generator = testing::register_test(
    "documentation_generator",
    "102_documentation_generator: stub status check",
    &run_documentation_generator);

}
