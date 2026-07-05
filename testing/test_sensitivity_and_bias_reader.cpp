#include "test_runner.hpp"
#include "../modules/508_sensitivity_and_bias_reader/sensitivity_and_bias_reader.hpp"

namespace {

testing::TestOutcome run_sensitivity_and_bias_reader() {
    sensitivity_and_bias_reader::init();
    auto s = sensitivity_and_bias_reader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sensitivity_and_bias_reader::shutdown();
    return testing::ok();
}

const int _reg_sensitivity_and_bias_reader = testing::register_test(
    "sensitivity_and_bias_reader",
    "508_sensitivity_and_bias_reader: stub status check",
    &run_sensitivity_and_bias_reader);

}
