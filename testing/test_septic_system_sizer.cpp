#include "test_runner.hpp"
#include "../modules/1065_septic_system_sizer/septic_system_sizer.hpp"

namespace {

testing::TestOutcome run_septic_system_sizer() {
    septic_system_sizer::init();
    auto s = septic_system_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    septic_system_sizer::shutdown();
    return testing::ok();
}

const int _reg_septic_system_sizer = testing::register_test(
    "septic_system_sizer",
    "1065_septic_system_sizer: stub status check",
    &run_septic_system_sizer);

}
