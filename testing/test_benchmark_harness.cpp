#include "test_runner.hpp"
#include "../modules/100_benchmark_harness/benchmark_harness.hpp"

namespace {

testing::TestOutcome run_benchmark_harness() {
    benchmark_harness::init();
    auto s = benchmark_harness::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    benchmark_harness::shutdown();
    return testing::ok();
}

const int _reg_benchmark_harness = testing::register_test(
    "benchmark_harness",
    "100_benchmark_harness: stub status check",
    &run_benchmark_harness);

}
