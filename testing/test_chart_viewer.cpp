#include "test_runner.hpp"
#include "../modules/1170_chart_viewer/chart_viewer.hpp"

namespace {

testing::TestOutcome run_chart_viewer() {
    chart_viewer::init();
    auto s = chart_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    chart_viewer::shutdown();
    return testing::ok();
}

const int _reg_chart_viewer = testing::register_test(
    "chart_viewer",
    "1170_chart_viewer: stub status check",
    &run_chart_viewer);

}
