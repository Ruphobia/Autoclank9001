#include "test_runner.hpp"
#include "../modules/1011_sound_transmission_loss_calculator/sound_transmission_loss_calculator.hpp"

namespace {

testing::TestOutcome run_sound_transmission_loss_calculator() {
    sound_transmission_loss_calculator::init();
    auto s = sound_transmission_loss_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sound_transmission_loss_calculator::shutdown();
    return testing::ok();
}

const int _reg_sound_transmission_loss_calculator = testing::register_test(
    "sound_transmission_loss_calculator",
    "1011_sound_transmission_loss_calculator: stub status check",
    &run_sound_transmission_loss_calculator);

}
