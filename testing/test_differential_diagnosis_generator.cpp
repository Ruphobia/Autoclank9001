#include "test_runner.hpp"
#include "../modules/1408_differential_diagnosis_generator/differential_diagnosis_generator.hpp"

namespace {

testing::TestOutcome run_differential_diagnosis_generator() {
    differential_diagnosis_generator::init();
    auto s = differential_diagnosis_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    differential_diagnosis_generator::shutdown();
    return testing::ok();
}

const int _reg_differential_diagnosis_generator = testing::register_test(
    "differential_diagnosis_generator",
    "1408_differential_diagnosis_generator: stub status check",
    &run_differential_diagnosis_generator);

}
