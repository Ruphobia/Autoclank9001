// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/917_error_propagation_tool/error_propagation_tool.hpp"

namespace {

testing::TestOutcome run_error_propagation_tool() {
    error_propagation_tool::init();
    auto s = error_propagation_tool::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    error_propagation_tool::shutdown();
    return testing::ok();
}

const int _reg_error_propagation_tool = testing::register_test(
    "error_propagation_tool",
    "917_error_propagation_tool: stub status check",
    &run_error_propagation_tool);

}
