#include "test_runner.hpp"
#include "../modules/1059_pump_curve_matcher/pump_curve_matcher.hpp"

namespace {

testing::TestOutcome run_pump_curve_matcher() {
    pump_curve_matcher::init();
    auto s = pump_curve_matcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pump_curve_matcher::shutdown();
    return testing::ok();
}

const int _reg_pump_curve_matcher = testing::register_test(
    "pump_curve_matcher",
    "1059_pump_curve_matcher: stub status check",
    &run_pump_curve_matcher);

}
