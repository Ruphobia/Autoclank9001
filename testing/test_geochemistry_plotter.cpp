// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1589_geochemistry_plotter/geochemistry_plotter.hpp"

namespace {

testing::TestOutcome run_geochemistry_plotter() {
    geochemistry_plotter::init();
    auto s = geochemistry_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geochemistry_plotter::shutdown();
    return testing::ok();
}

const int _reg_geochemistry_plotter = testing::register_test(
    "geochemistry_plotter",
    "1589_geochemistry_plotter: stub status check",
    &run_geochemistry_plotter);

}
