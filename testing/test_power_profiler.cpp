#include "test_runner.hpp"
#include "../modules/198_power_profiler/power_profiler.hpp"

namespace {

testing::TestOutcome run_power_profiler() {
    power_profiler::init();
    auto s = power_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    power_profiler::shutdown();
    return testing::ok();
}

const int _reg_power_profiler = testing::register_test(
    "power_profiler",
    "198_power_profiler: stub status check",
    &run_power_profiler);

}
