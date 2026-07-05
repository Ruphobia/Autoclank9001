// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/230_explain_visualizer/explain_visualizer.hpp"

namespace {

testing::TestOutcome run_explain_visualizer() {
    explain_visualizer::init();
    auto s = explain_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    explain_visualizer::shutdown();
    return testing::ok();
}

const int _reg_explain_visualizer = testing::register_test(
    "explain_visualizer",
    "230_explain_visualizer: stub status check",
    &run_explain_visualizer);

}
