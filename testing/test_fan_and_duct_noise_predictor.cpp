#include "test_runner.hpp"
#include "../modules/1030_fan_and_duct_noise_predictor/fan_and_duct_noise_predictor.hpp"

namespace {

testing::TestOutcome run_fan_and_duct_noise_predictor() {
    fan_and_duct_noise_predictor::init();
    auto s = fan_and_duct_noise_predictor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    fan_and_duct_noise_predictor::shutdown();
    return testing::ok();
}

const int _reg_fan_and_duct_noise_predictor = testing::register_test(
    "fan_and_duct_noise_predictor",
    "1030_fan_and_duct_noise_predictor: stub status check",
    &run_fan_and_duct_noise_predictor);

}
