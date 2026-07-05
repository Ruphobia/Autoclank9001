#include "test_runner.hpp"
#include "../modules/986_ashby_chart_plotter/ashby_chart_plotter.hpp"

namespace {

testing::TestOutcome run_ashby_chart_plotter() {
    ashby_chart_plotter::init();
    auto s = ashby_chart_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ashby_chart_plotter::shutdown();
    return testing::ok();
}

const int _reg_ashby_chart_plotter = testing::register_test(
    "ashby_chart_plotter",
    "986_ashby_chart_plotter: stub status check",
    &run_ashby_chart_plotter);

}
