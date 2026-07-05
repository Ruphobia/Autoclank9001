#include "test_runner.hpp"
#include "../modules/092_profiler/profiler.hpp"

namespace {

testing::TestOutcome run_profiler() {
    profiler::init();
    auto s = profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    profiler::shutdown();
    return testing::ok();
}

const int _reg_profiler = testing::register_test(
    "profiler",
    "092_profiler: stub status check",
    &run_profiler);

}
