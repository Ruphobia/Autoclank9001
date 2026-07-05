// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/764_source_layer_visualizer/source_layer_visualizer.hpp"

namespace {

testing::TestOutcome run_source_layer_visualizer() {
    source_layer_visualizer::init();
    auto s = source_layer_visualizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    source_layer_visualizer::shutdown();
    return testing::ok();
}

const int _reg_source_layer_visualizer = testing::register_test(
    "source_layer_visualizer",
    "764_source_layer_visualizer: stub status check",
    &run_source_layer_visualizer);

}
