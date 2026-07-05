#include "test_runner.hpp"
#include "../modules/1136_re_entry_calculator/re_entry_calculator.hpp"

namespace {

testing::TestOutcome run_re_entry_calculator() {
    re_entry_calculator::init();
    auto s = re_entry_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    re_entry_calculator::shutdown();
    return testing::ok();
}

const int _reg_re_entry_calculator = testing::register_test(
    "re_entry_calculator",
    "1136_re_entry_calculator: stub status check",
    &run_re_entry_calculator);

}
