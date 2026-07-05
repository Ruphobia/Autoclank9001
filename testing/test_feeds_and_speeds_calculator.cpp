#include "test_runner.hpp"
#include "../modules/930_feeds_and_speeds_calculator/feeds_and_speeds_calculator.hpp"

namespace {

testing::TestOutcome run_feeds_and_speeds_calculator() {
    feeds_and_speeds_calculator::init();
    auto s = feeds_and_speeds_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    feeds_and_speeds_calculator::shutdown();
    return testing::ok();
}

const int _reg_feeds_and_speeds_calculator = testing::register_test(
    "feeds_and_speeds_calculator",
    "930_feeds_and_speeds_calculator: stub status check",
    &run_feeds_and_speeds_calculator);

}
