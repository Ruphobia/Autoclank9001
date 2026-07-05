// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1111_pipe_stress_and_flexibility_analyzer/pipe_stress_and_flexibility_analyzer.hpp"

namespace {

testing::TestOutcome run_pipe_stress_and_flexibility_analyzer() {
    pipe_stress_and_flexibility_analyzer::init();
    auto s = pipe_stress_and_flexibility_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pipe_stress_and_flexibility_analyzer::shutdown();
    return testing::ok();
}

const int _reg_pipe_stress_and_flexibility_analyzer = testing::register_test(
    "pipe_stress_and_flexibility_analyzer",
    "1111_pipe_stress_and_flexibility_analyzer: stub status check",
    &run_pipe_stress_and_flexibility_analyzer);

}
