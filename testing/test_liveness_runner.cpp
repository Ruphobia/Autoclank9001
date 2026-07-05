#include "test_runner.hpp"
#include "../modules/605_liveness_runner/liveness_runner.hpp"

namespace {

testing::TestOutcome run_liveness_runner() {
    liveness_runner::init();
    auto s = liveness_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    liveness_runner::shutdown();
    return testing::ok();
}

const int _reg_liveness_runner = testing::register_test(
    "liveness_runner",
    "605_liveness_runner: stub status check",
    &run_liveness_runner);

}
