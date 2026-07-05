#include "test_runner.hpp"
#include "../modules/2084_expected_value_model_fitter/expected_value_model_fitter.hpp"

namespace {

testing::TestOutcome run_expected_value_model_fitter() {
    expected_value_model_fitter::init();
    auto s = expected_value_model_fitter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    expected_value_model_fitter::shutdown();
    return testing::ok();
}

const int _reg_expected_value_model_fitter = testing::register_test(
    "expected_value_model_fitter",
    "2084_expected_value_model_fitter: stub status check",
    &run_expected_value_model_fitter);

}
