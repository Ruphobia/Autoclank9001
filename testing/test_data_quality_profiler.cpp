#include "test_runner.hpp"
#include "../modules/669_data_quality_profiler/data_quality_profiler.hpp"

namespace {

testing::TestOutcome run_data_quality_profiler() {
    data_quality_profiler::init();
    auto s = data_quality_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    data_quality_profiler::shutdown();
    return testing::ok();
}

const int _reg_data_quality_profiler = testing::register_test(
    "data_quality_profiler",
    "669_data_quality_profiler: stub status check",
    &run_data_quality_profiler);

}
