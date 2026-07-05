#include "test_runner.hpp"
#include "../modules/1947_dice_roller_with_macros/dice_roller_with_macros.hpp"

namespace {

testing::TestOutcome run_dice_roller_with_macros() {
    dice_roller_with_macros::init();
    auto s = dice_roller_with_macros::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dice_roller_with_macros::shutdown();
    return testing::ok();
}

const int _reg_dice_roller_with_macros = testing::register_test(
    "dice_roller_with_macros",
    "1947_dice_roller_with_macros: stub status check",
    &run_dice_roller_with_macros);

}
