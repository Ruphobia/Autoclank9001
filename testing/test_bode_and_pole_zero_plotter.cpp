#include "test_runner.hpp"
#include "../modules/1728_bode_and_pole_zero_plotter/bode_and_pole_zero_plotter.hpp"

namespace {

testing::TestOutcome run_bode_and_pole_zero_plotter() {
    bode_and_pole_zero_plotter::init();
    auto s = bode_and_pole_zero_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bode_and_pole_zero_plotter::shutdown();
    return testing::ok();
}

const int _reg_bode_and_pole_zero_plotter = testing::register_test(
    "bode_and_pole_zero_plotter",
    "1728_bode_and_pole_zero_plotter: stub status check",
    &run_bode_and_pole_zero_plotter);

}
