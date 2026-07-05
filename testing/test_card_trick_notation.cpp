#include "test_runner.hpp"
#include "../modules/1930_card_trick_notation/card_trick_notation.hpp"

namespace {

testing::TestOutcome run_card_trick_notation() {
    card_trick_notation::init();
    auto s = card_trick_notation::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    card_trick_notation::shutdown();
    return testing::ok();
}

const int _reg_card_trick_notation = testing::register_test(
    "card_trick_notation",
    "1930_card_trick_notation: stub status check",
    &run_card_trick_notation);

}
