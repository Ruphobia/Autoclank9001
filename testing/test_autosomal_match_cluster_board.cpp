// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2132_autosomal_match_cluster_board/autosomal_match_cluster_board.hpp"

namespace {

testing::TestOutcome run_autosomal_match_cluster_board() {
    autosomal_match_cluster_board::init();
    auto s = autosomal_match_cluster_board::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    autosomal_match_cluster_board::shutdown();
    return testing::ok();
}

const int _reg_autosomal_match_cluster_board = testing::register_test(
    "autosomal_match_cluster_board",
    "2132_autosomal_match_cluster_board: stub status check",
    &run_autosomal_match_cluster_board);

}
