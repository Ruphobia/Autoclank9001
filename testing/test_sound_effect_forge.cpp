#include "test_runner.hpp"
#include "../modules/292_sound_effect_forge/sound_effect_forge.hpp"

namespace {

testing::TestOutcome run_sound_effect_forge() {
    sound_effect_forge::init();
    auto s = sound_effect_forge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sound_effect_forge::shutdown();
    return testing::ok();
}

const int _reg_sound_effect_forge = testing::register_test(
    "sound_effect_forge",
    "292_sound_effect_forge: stub status check",
    &run_sound_effect_forge);

}
