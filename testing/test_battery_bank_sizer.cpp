#include "test_runner.hpp"
#include "../modules/1280_battery_bank_sizer/battery_bank_sizer.hpp"

namespace {

testing::TestOutcome run_battery_bank_sizer() {
    battery_bank_sizer::init();
    auto s = battery_bank_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    battery_bank_sizer::shutdown();
    return testing::ok();
}

const int _reg_battery_bank_sizer = testing::register_test(
    "battery_bank_sizer",
    "1280_battery_bank_sizer: stub status check",
    &run_battery_bank_sizer);

}
