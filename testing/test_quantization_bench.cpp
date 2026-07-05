#include "test_runner.hpp"
#include "../modules/674_quantization_bench/quantization_bench.hpp"

namespace {

testing::TestOutcome run_quantization_bench() {
    quantization_bench::init();
    auto s = quantization_bench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    quantization_bench::shutdown();
    return testing::ok();
}

const int _reg_quantization_bench = testing::register_test(
    "quantization_bench",
    "674_quantization_bench: stub status check",
    &run_quantization_bench);

}
