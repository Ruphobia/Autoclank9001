#include "test_runner.hpp"
#include "../modules/1900_ink_library_and_mix_calculator/ink_library_and_mix_calculator.hpp"

namespace {

testing::TestOutcome run_ink_library_and_mix_calculator() {
    ink_library_and_mix_calculator::init();
    auto s = ink_library_and_mix_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ink_library_and_mix_calculator::shutdown();
    return testing::ok();
}

const int _reg_ink_library_and_mix_calculator = testing::register_test(
    "ink_library_and_mix_calculator",
    "1900_ink_library_and_mix_calculator: stub status check",
    &run_ink_library_and_mix_calculator);

}
