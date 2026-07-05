#include "test_runner.hpp"
#include "../modules/1369_growth_curve_plotter/growth_curve_plotter.hpp"

namespace {

testing::TestOutcome run_growth_curve_plotter() {
    growth_curve_plotter::init();
    auto s = growth_curve_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    growth_curve_plotter::shutdown();
    return testing::ok();
}

const int _reg_growth_curve_plotter = testing::register_test(
    "growth_curve_plotter",
    "1369_growth_curve_plotter: stub status check",
    &run_growth_curve_plotter);

}
