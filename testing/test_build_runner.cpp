#include "test_runner.hpp"
#include "../modules/089_build_runner/build_runner.hpp"

namespace {

testing::TestOutcome run_build_runner() {
    build_runner::init();
    auto s = build_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    build_runner::shutdown();
    return testing::ok();
}

const int _reg_build_runner = testing::register_test(
    "build_runner",
    "089_build_runner: stub status check",
    &run_build_runner);

}
