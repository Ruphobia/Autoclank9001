// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/218_bsp_and_board_generator/bsp_and_board_generator.hpp"

namespace {

testing::TestOutcome run_bsp_and_board_generator() {
    bsp_and_board_generator::init();
    auto s = bsp_and_board_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bsp_and_board_generator::shutdown();
    return testing::ok();
}

const int _reg_bsp_and_board_generator = testing::register_test(
    "bsp_and_board_generator",
    "218_bsp_and_board_generator: stub status check",
    &run_bsp_and_board_generator);

}
