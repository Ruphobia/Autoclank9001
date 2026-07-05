#include "test_runner.hpp"
#include "../modules/197_trace_and_profiler/trace_and_profiler.hpp"

namespace {

testing::TestOutcome run_trace_and_profiler() {
    trace_and_profiler::init();
    auto s = trace_and_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    trace_and_profiler::shutdown();
    return testing::ok();
}

const int _reg_trace_and_profiler = testing::register_test(
    "trace_and_profiler",
    "197_trace_and_profiler: stub status check",
    &run_trace_and_profiler);

}
