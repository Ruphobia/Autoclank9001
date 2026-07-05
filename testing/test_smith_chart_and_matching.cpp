// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1186_smith_chart_and_matching/smith_chart_and_matching.hpp"

namespace {

testing::TestOutcome run_smith_chart_and_matching() {
    smith_chart_and_matching::init();
    auto s = smith_chart_and_matching::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    smith_chart_and_matching::shutdown();
    return testing::ok();
}

const int _reg_smith_chart_and_matching = testing::register_test(
    "smith_chart_and_matching",
    "1186_smith_chart_and_matching: stub status check",
    &run_smith_chart_and_matching);

}
