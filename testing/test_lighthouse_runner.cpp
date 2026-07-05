#include "test_runner.hpp"
#include "../modules/120_lighthouse_runner/lighthouse_runner.hpp"

namespace {

testing::TestOutcome run_lighthouse_runner() {
    lighthouse_runner::init();
    auto s = lighthouse_runner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lighthouse_runner::shutdown();
    return testing::ok();
}

const int _reg_lighthouse_runner = testing::register_test(
    "lighthouse_runner",
    "120_lighthouse_runner: stub status check",
    &run_lighthouse_runner);

}
