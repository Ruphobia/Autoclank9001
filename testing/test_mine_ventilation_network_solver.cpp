// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1096_mine_ventilation_network_solver/mine_ventilation_network_solver.hpp"

namespace {

testing::TestOutcome run_mine_ventilation_network_solver() {
    mine_ventilation_network_solver::init();
    auto s = mine_ventilation_network_solver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mine_ventilation_network_solver::shutdown();
    return testing::ok();
}

const int _reg_mine_ventilation_network_solver = testing::register_test(
    "mine_ventilation_network_solver",
    "1096_mine_ventilation_network_solver: stub status check",
    &run_mine_ventilation_network_solver);

}
