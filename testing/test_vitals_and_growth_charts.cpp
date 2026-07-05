#include "test_runner.hpp"
#include "../modules/1418_vitals_and_growth_charts/vitals_and_growth_charts.hpp"

namespace {

testing::TestOutcome run_vitals_and_growth_charts() {
    vitals_and_growth_charts::init();
    auto s = vitals_and_growth_charts::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    vitals_and_growth_charts::shutdown();
    return testing::ok();
}

const int _reg_vitals_and_growth_charts = testing::register_test(
    "vitals_and_growth_charts",
    "1418_vitals_and_growth_charts: stub status check",
    &run_vitals_and_growth_charts);

}
