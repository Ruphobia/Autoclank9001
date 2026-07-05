// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1586_watershed_analyzer/watershed_analyzer.hpp"

namespace {

testing::TestOutcome run_watershed_analyzer() {
    watershed_analyzer::init();
    auto s = watershed_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    watershed_analyzer::shutdown();
    return testing::ok();
}

const int _reg_watershed_analyzer = testing::register_test(
    "watershed_analyzer",
    "1586_watershed_analyzer: stub status check",
    &run_watershed_analyzer);

}
