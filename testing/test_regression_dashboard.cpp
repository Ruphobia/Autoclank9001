#include "test_runner.hpp"
#include "../modules/1445_regression_dashboard/regression_dashboard.hpp"

namespace {

testing::TestOutcome run_regression_dashboard() {
    regression_dashboard::init();
    auto s = regression_dashboard::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    regression_dashboard::shutdown();
    return testing::ok();
}

const int _reg_regression_dashboard = testing::register_test(
    "regression_dashboard",
    "1445_regression_dashboard: stub status check",
    &run_regression_dashboard);

}
