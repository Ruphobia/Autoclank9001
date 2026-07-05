#include "test_runner.hpp"
#include "../modules/176_gradle_and_xcode_build_profiler/gradle_and_xcode_build_profiler.hpp"

namespace {

testing::TestOutcome run_gradle_and_xcode_build_profiler() {
    gradle_and_xcode_build_profiler::init();
    auto s = gradle_and_xcode_build_profiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gradle_and_xcode_build_profiler::shutdown();
    return testing::ok();
}

const int _reg_gradle_and_xcode_build_profiler = testing::register_test(
    "gradle_and_xcode_build_profiler",
    "176_gradle_and_xcode_build_profiler: stub status check",
    &run_gradle_and_xcode_build_profiler);

}
