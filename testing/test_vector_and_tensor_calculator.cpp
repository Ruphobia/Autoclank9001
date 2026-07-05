#include "test_runner.hpp"
#include "../modules/901_vector_and_tensor_calculator/vector_and_tensor_calculator.hpp"

namespace {

testing::TestOutcome run_vector_and_tensor_calculator() {
    vector_and_tensor_calculator::init();
    auto s = vector_and_tensor_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vector_and_tensor_calculator::shutdown();
    return testing::ok();
}

const int _reg_vector_and_tensor_calculator = testing::register_test(
    "vector_and_tensor_calculator",
    "901_vector_and_tensor_calculator: stub status check",
    &run_vector_and_tensor_calculator);

}
