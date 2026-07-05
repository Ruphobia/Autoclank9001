#include "test_runner.hpp"
#include "../modules/1851_glass_coe_and_color_match/glass_coe_and_color_match.hpp"

namespace {

testing::TestOutcome run_glass_coe_and_color_match() {
    glass_coe_and_color_match::init();
    auto s = glass_coe_and_color_match::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    glass_coe_and_color_match::shutdown();
    return testing::ok();
}

const int _reg_glass_coe_and_color_match = testing::register_test(
    "glass_coe_and_color_match",
    "1851_glass_coe_and_color_match: stub status check",
    &run_glass_coe_and_color_match);

}
