#include "test_runner.hpp"
#include "../modules/856_bench_instrument_bridge/bench_instrument_bridge.hpp"

namespace {

testing::TestOutcome run_bench_instrument_bridge() {
    bench_instrument_bridge::init();
    auto s = bench_instrument_bridge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bench_instrument_bridge::shutdown();
    return testing::ok();
}

const int _reg_bench_instrument_bridge = testing::register_test(
    "bench_instrument_bridge",
    "856_bench_instrument_bridge: stub status check",
    &run_bench_instrument_bridge);

}
