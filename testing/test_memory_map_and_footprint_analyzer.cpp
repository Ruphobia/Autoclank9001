// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/190_memory_map_and_footprint_analyzer/memory_map_and_footprint_analyzer.hpp"

namespace {

testing::TestOutcome run_memory_map_and_footprint_analyzer() {
    memory_map_and_footprint_analyzer::init();
    auto s = memory_map_and_footprint_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    memory_map_and_footprint_analyzer::shutdown();
    return testing::ok();
}

const int _reg_memory_map_and_footprint_analyzer = testing::register_test(
    "memory_map_and_footprint_analyzer",
    "190_memory_map_and_footprint_analyzer: stub status check",
    &run_memory_map_and_footprint_analyzer);

}
