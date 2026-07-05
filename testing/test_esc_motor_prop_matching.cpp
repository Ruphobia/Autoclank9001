#include "test_runner.hpp"
#include "../modules/2193_esc_motor_prop_matching/esc_motor_prop_matching.hpp"

namespace {

testing::TestOutcome run_esc_motor_prop_matching() {
    esc_motor_prop_matching::init();
    auto s = esc_motor_prop_matching::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    esc_motor_prop_matching::shutdown();
    return testing::ok();
}

const int _reg_esc_motor_prop_matching = testing::register_test(
    "esc_motor_prop_matching",
    "2193_esc_motor_prop_matching: stub status check",
    &run_esc_motor_prop_matching);

}
