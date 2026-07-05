// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/623_figure_and_diagram_extractor/figure_and_diagram_extractor.hpp"

namespace {

testing::TestOutcome run_figure_and_diagram_extractor() {
    figure_and_diagram_extractor::init();
    auto s = figure_and_diagram_extractor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    figure_and_diagram_extractor::shutdown();
    return testing::ok();
}

const int _reg_figure_and_diagram_extractor = testing::register_test(
    "figure_and_diagram_extractor",
    "623_figure_and_diagram_extractor: stub status check",
    &run_figure_and_diagram_extractor);

}
