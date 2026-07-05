#include "test_runner.hpp"
#include "../modules/1556_sound_speed_profile_builder/sound_speed_profile_builder.hpp"

namespace {

testing::TestOutcome run_sound_speed_profile_builder() {
    sound_speed_profile_builder::init();
    auto s = sound_speed_profile_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sound_speed_profile_builder::shutdown();
    return testing::ok();
}

const int _reg_sound_speed_profile_builder = testing::register_test(
    "sound_speed_profile_builder",
    "1556_sound_speed_profile_builder: stub status check",
    &run_sound_speed_profile_builder);

}
