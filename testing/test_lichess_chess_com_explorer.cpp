// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1966_lichess_chess_com_explorer/lichess_chess_com_explorer.hpp"

namespace {

testing::TestOutcome run_lichess_chess_com_explorer() {
    lichess_chess_com_explorer::init();
    auto s = lichess_chess_com_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lichess_chess_com_explorer::shutdown();
    return testing::ok();
}

const int _reg_lichess_chess_com_explorer = testing::register_test(
    "lichess_chess_com_explorer",
    "1966_lichess_chess_com_explorer: stub status check",
    &run_lichess_chess_com_explorer);

}
