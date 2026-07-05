#include "test_runner.hpp"
#include "../modules/1601_soil_test_interpreter/soil_test_interpreter.hpp"

namespace {

testing::TestOutcome run_soil_test_interpreter() {
    soil_test_interpreter::init();
    auto s = soil_test_interpreter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    soil_test_interpreter::shutdown();
    return testing::ok();
}

const int _reg_soil_test_interpreter = testing::register_test(
    "soil_test_interpreter",
    "1601_soil_test_interpreter: stub status check",
    &run_soil_test_interpreter);

}
