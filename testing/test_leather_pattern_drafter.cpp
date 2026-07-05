#include "test_runner.hpp"
#include "../modules/1853_leather_pattern_drafter/leather_pattern_drafter.hpp"

namespace {

testing::TestOutcome run_leather_pattern_drafter() {
    leather_pattern_drafter::init();
    auto s = leather_pattern_drafter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    leather_pattern_drafter::shutdown();
    return testing::ok();
}

const int _reg_leather_pattern_drafter = testing::register_test(
    "leather_pattern_drafter",
    "1853_leather_pattern_drafter: stub status check",
    &run_leather_pattern_drafter);

}
