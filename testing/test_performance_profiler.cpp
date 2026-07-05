// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/281_performance_profiler/performance_profiler.hpp"

namespace {

testing::TestOutcome run_performance_profiler() {
    performance_profiler::init();
    auto s = performance_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    performance_profiler::shutdown();
    return testing::ok();
}

const int _reg_performance_profiler = testing::register_test(
    "performance_profiler",
    "281_performance_profiler: stub status check",
    &run_performance_profiler);

}
