#include "test_runner.hpp"
#include "../modules/1736_noise_reduction_and_restoration/noise_reduction_and_restoration.hpp"

namespace {

testing::TestOutcome run_noise_reduction_and_restoration() {
    noise_reduction_and_restoration::init();
    auto s = noise_reduction_and_restoration::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    noise_reduction_and_restoration::shutdown();
    return testing::ok();
}

const int _reg_noise_reduction_and_restoration = testing::register_test(
    "noise_reduction_and_restoration",
    "1736_noise_reduction_and_restoration: stub status check",
    &run_noise_reduction_and_restoration);

}
