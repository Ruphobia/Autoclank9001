#include "test_runner.hpp"
#include "../modules/440_burndown_and_burnup_charts/burndown_and_burnup_charts.hpp"

namespace {

testing::TestOutcome run_burndown_and_burnup_charts() {
    burndown_and_burnup_charts::init();
    auto s = burndown_and_burnup_charts::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    burndown_and_burnup_charts::shutdown();
    return testing::ok();
}

const int _reg_burndown_and_burnup_charts = testing::register_test(
    "burndown_and_burnup_charts",
    "440_burndown_and_burnup_charts: stub status check",
    &run_burndown_and_burnup_charts);

}
