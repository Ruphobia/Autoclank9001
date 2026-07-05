// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1463_metar_synop_station_plotter/metar_synop_station_plotter.hpp"

namespace {

testing::TestOutcome run_metar_synop_station_plotter() {
    metar_synop_station_plotter::init();
    auto s = metar_synop_station_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    metar_synop_station_plotter::shutdown();
    return testing::ok();
}

const int _reg_metar_synop_station_plotter = testing::register_test(
    "metar_synop_station_plotter",
    "1463_metar_synop_station_plotter: stub status check",
    &run_metar_synop_station_plotter);

}
