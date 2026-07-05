#include "test_runner.hpp"
#include "../modules/2176_breathing_pacer/breathing_pacer.hpp"

namespace {

testing::TestOutcome run_breathing_pacer() {
    breathing_pacer::init();
    auto s = breathing_pacer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    breathing_pacer::shutdown();
    return testing::ok();
}

const int _reg_breathing_pacer = testing::register_test(
    "breathing_pacer",
    "2176_breathing_pacer: stub status check",
    &run_breathing_pacer);

}
