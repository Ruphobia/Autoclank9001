// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/988_ttt_cct_plotter/ttt_cct_plotter.hpp"

namespace {

testing::TestOutcome run_ttt_cct_plotter() {
    ttt_cct_plotter::init();
    auto s = ttt_cct_plotter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ttt_cct_plotter::shutdown();
    return testing::ok();
}

const int _reg_ttt_cct_plotter = testing::register_test(
    "ttt_cct_plotter",
    "988_ttt_cct_plotter: stub status check",
    &run_ttt_cct_plotter);

}
