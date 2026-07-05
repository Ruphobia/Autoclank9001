#include "test_runner.hpp"
#include "../modules/1476_ephemeris_calculator/ephemeris_calculator.hpp"

namespace {

testing::TestOutcome run_ephemeris_calculator() {
    ephemeris_calculator::init();
    auto s = ephemeris_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ephemeris_calculator::shutdown();
    return testing::ok();
}

const int _reg_ephemeris_calculator = testing::register_test(
    "ephemeris_calculator",
    "1476_ephemeris_calculator: stub status check",
    &run_ephemeris_calculator);

}
