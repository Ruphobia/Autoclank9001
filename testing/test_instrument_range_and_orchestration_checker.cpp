#include "test_runner.hpp"
#include "../modules/1721_instrument_range_and_orchestration_checker/instrument_range_and_orchestration_checker.hpp"

namespace {

testing::TestOutcome run_instrument_range_and_orchestration_checker() {
    instrument_range_and_orchestration_checker::init();
    auto s = instrument_range_and_orchestration_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    instrument_range_and_orchestration_checker::shutdown();
    return testing::ok();
}

const int _reg_instrument_range_and_orchestration_checker = testing::register_test(
    "instrument_range_and_orchestration_checker",
    "1721_instrument_range_and_orchestration_checker: stub status check",
    &run_instrument_range_and_orchestration_checker);

}
