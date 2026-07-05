#include "test_runner.hpp"
#include "../modules/1410_ecg_caliper/ecg_caliper.hpp"

namespace {

testing::TestOutcome run_ecg_caliper() {
    ecg_caliper::init();
    auto s = ecg_caliper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ecg_caliper::shutdown();
    return testing::ok();
}

const int _reg_ecg_caliper = testing::register_test(
    "ecg_caliper",
    "1410_ecg_caliper: stub status check",
    &run_ecg_caliper);

}
