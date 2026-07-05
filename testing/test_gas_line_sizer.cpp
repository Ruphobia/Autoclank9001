#include "test_runner.hpp"
#include "../modules/2022_gas_line_sizer/gas_line_sizer.hpp"

namespace {

testing::TestOutcome run_gas_line_sizer() {
    gas_line_sizer::init();
    auto s = gas_line_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    gas_line_sizer::shutdown();
    return testing::ok();
}

const int _reg_gas_line_sizer = testing::register_test(
    "gas_line_sizer",
    "2022_gas_line_sizer: stub status check",
    &run_gas_line_sizer);

}
