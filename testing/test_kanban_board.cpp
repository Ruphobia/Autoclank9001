#include "test_runner.hpp"
#include "../modules/419_kanban_board/kanban_board.hpp"

namespace {

testing::TestOutcome run_kanban_board() {
    kanban_board::init();
    auto s = kanban_board::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    kanban_board::shutdown();
    return testing::ok();
}

const int _reg_kanban_board = testing::register_test(
    "kanban_board",
    "419_kanban_board: stub status check",
    &run_kanban_board);

}
