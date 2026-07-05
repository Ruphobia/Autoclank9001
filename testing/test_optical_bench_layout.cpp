#include "test_runner.hpp"
#include "../modules/880_optical_bench_layout/optical_bench_layout.hpp"

namespace {

testing::TestOutcome run_optical_bench_layout() {
    optical_bench_layout::init();
    auto s = optical_bench_layout::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    optical_bench_layout::shutdown();
    return testing::ok();
}

const int _reg_optical_bench_layout = testing::register_test(
    "optical_bench_layout",
    "880_optical_bench_layout: stub status check",
    &run_optical_bench_layout);

}
