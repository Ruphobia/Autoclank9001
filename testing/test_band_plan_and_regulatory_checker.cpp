#include "test_runner.hpp"
#include "../modules/1194_band_plan_and_regulatory_checker/band_plan_and_regulatory_checker.hpp"

namespace {

testing::TestOutcome run_band_plan_and_regulatory_checker() {
    band_plan_and_regulatory_checker::init();
    auto s = band_plan_and_regulatory_checker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    band_plan_and_regulatory_checker::shutdown();
    return testing::ok();
}

const int _reg_band_plan_and_regulatory_checker = testing::register_test(
    "band_plan_and_regulatory_checker",
    "1194_band_plan_and_regulatory_checker: stub status check",
    &run_band_plan_and_regulatory_checker);

}
