#include "test_runner.hpp"
#include "../modules/1061_tr_55_runoff_calculator/tr_55_runoff_calculator.hpp"

namespace {

testing::TestOutcome run_tr_55_runoff_calculator() {
    tr_55_runoff_calculator::init();
    auto s = tr_55_runoff_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tr_55_runoff_calculator::shutdown();
    return testing::ok();
}

const int _reg_tr_55_runoff_calculator = testing::register_test(
    "tr_55_runoff_calculator",
    "1061_tr_55_runoff_calculator: stub status check",
    &run_tr_55_runoff_calculator);

}
