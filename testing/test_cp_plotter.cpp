#include "test_runner.hpp"
#include "../modules/1151_cp_plotter/cp_plotter.hpp"

namespace {

testing::TestOutcome run_cp_plotter() {
    cp_plotter::init();
    auto s = cp_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cp_plotter::shutdown();
    return testing::ok();
}

const int _reg_cp_plotter = testing::register_test(
    "cp_plotter",
    "1151_cp_plotter: stub status check",
    &run_cp_plotter);

}
