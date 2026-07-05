#include "test_runner.hpp"
#include "../modules/868_reverse_engineering_board_scanner/reverse_engineering_board_scanner.hpp"

namespace {

testing::TestOutcome run_reverse_engineering_board_scanner() {
    reverse_engineering_board_scanner::init();
    auto s = reverse_engineering_board_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reverse_engineering_board_scanner::shutdown();
    return testing::ok();
}

const int _reg_reverse_engineering_board_scanner = testing::register_test(
    "reverse_engineering_board_scanner",
    "868_reverse_engineering_board_scanner: stub status check",
    &run_reverse_engineering_board_scanner);

}
