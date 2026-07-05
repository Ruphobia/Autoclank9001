#include "test_runner.hpp"
#include "../modules/787_sound_change_applier/sound_change_applier.hpp"

namespace {

testing::TestOutcome run_sound_change_applier() {
    sound_change_applier::init();
    auto s = sound_change_applier::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sound_change_applier::shutdown();
    return testing::ok();
}

const int _reg_sound_change_applier = testing::register_test(
    "sound_change_applier",
    "787_sound_change_applier: stub status check",
    &run_sound_change_applier);

}
