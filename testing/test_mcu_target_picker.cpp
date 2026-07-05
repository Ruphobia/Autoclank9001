#include "test_runner.hpp"
#include "../modules/177_mcu_target_picker/mcu_target_picker.hpp"

namespace {

testing::TestOutcome run_mcu_target_picker() {
    mcu_target_picker::init();
    auto s = mcu_target_picker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mcu_target_picker::shutdown();
    return testing::ok();
}

const int _reg_mcu_target_picker = testing::register_test(
    "mcu_target_picker",
    "177_mcu_target_picker: stub status check",
    &run_mcu_target_picker);

}
