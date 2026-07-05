#include "test_runner.hpp"
#include "../modules/1972_hand_history_parser/hand_history_parser.hpp"

namespace {

testing::TestOutcome run_hand_history_parser() {
    hand_history_parser::init();
    auto s = hand_history_parser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hand_history_parser::shutdown();
    return testing::ok();
}

const int _reg_hand_history_parser = testing::register_test(
    "hand_history_parser",
    "1972_hand_history_parser: stub status check",
    &run_hand_history_parser);

}
