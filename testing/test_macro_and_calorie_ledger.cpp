#include "test_runner.hpp"
#include "../modules/2074_macro_and_calorie_ledger/macro_and_calorie_ledger.hpp"

namespace {

testing::TestOutcome run_macro_and_calorie_ledger() {
    macro_and_calorie_ledger::init();
    auto s = macro_and_calorie_ledger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    macro_and_calorie_ledger::shutdown();
    return testing::ok();
}

const int _reg_macro_and_calorie_ledger = testing::register_test(
    "macro_and_calorie_ledger",
    "2074_macro_and_calorie_ledger: stub status check",
    &run_macro_and_calorie_ledger);

}
