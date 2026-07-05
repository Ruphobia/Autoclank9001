#include "test_runner.hpp"
#include "../modules/905_ray_trace_optics_bench/ray_trace_optics_bench.hpp"

namespace {

testing::TestOutcome run_ray_trace_optics_bench() {
    ray_trace_optics_bench::init();
    auto s = ray_trace_optics_bench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ray_trace_optics_bench::shutdown();
    return testing::ok();
}

const int _reg_ray_trace_optics_bench = testing::register_test(
    "ray_trace_optics_bench",
    "905_ray_trace_optics_bench: stub status check",
    &run_ray_trace_optics_bench);

}
