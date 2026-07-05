#include "test_runner.hpp"
#include "../modules/254_cost_and_storage_profiler/cost_and_storage_profiler.hpp"

namespace {

testing::TestOutcome run_cost_and_storage_profiler() {
    cost_and_storage_profiler::init();
    auto s = cost_and_storage_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cost_and_storage_profiler::shutdown();
    return testing::ok();
}

const int _reg_cost_and_storage_profiler = testing::register_test(
    "cost_and_storage_profiler",
    "254_cost_and_storage_profiler: stub status check",
    &run_cost_and_storage_profiler);

}
