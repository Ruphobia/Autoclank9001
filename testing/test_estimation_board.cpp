#include "test_runner.hpp"
#include "../modules/443_estimation_board/estimation_board.hpp"

namespace {

testing::TestOutcome run_estimation_board() {
    estimation_board::init();
    auto s = estimation_board::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    estimation_board::shutdown();
    return testing::ok();
}

const int _reg_estimation_board = testing::register_test(
    "estimation_board",
    "443_estimation_board: stub status check",
    &run_estimation_board);

}
