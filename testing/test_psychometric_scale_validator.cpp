#include "test_runner.hpp"
#include "../modules/1428_psychometric_scale_validator/psychometric_scale_validator.hpp"

namespace {

testing::TestOutcome run_psychometric_scale_validator() {
    psychometric_scale_validator::init();
    auto s = psychometric_scale_validator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    psychometric_scale_validator::shutdown();
    return testing::ok();
}

const int _reg_psychometric_scale_validator = testing::register_test(
    "psychometric_scale_validator",
    "1428_psychometric_scale_validator: stub status check",
    &run_psychometric_scale_validator);

}
