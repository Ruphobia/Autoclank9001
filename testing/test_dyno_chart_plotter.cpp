#include "test_runner.hpp"
#include "../modules/1202_dyno_chart_plotter/dyno_chart_plotter.hpp"

namespace {

testing::TestOutcome run_dyno_chart_plotter() {
    dyno_chart_plotter::init();
    auto s = dyno_chart_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dyno_chart_plotter::shutdown();
    return testing::ok();
}

const int _reg_dyno_chart_plotter = testing::register_test(
    "dyno_chart_plotter",
    "1202_dyno_chart_plotter: stub status check",
    &run_dyno_chart_plotter);

}
