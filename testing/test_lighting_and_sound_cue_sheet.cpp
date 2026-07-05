#include "test_runner.hpp"
#include "../modules/1942_lighting_and_sound_cue_sheet/lighting_and_sound_cue_sheet.hpp"

namespace {

testing::TestOutcome run_lighting_and_sound_cue_sheet() {
    lighting_and_sound_cue_sheet::init();
    auto s = lighting_and_sound_cue_sheet::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lighting_and_sound_cue_sheet::shutdown();
    return testing::ok();
}

const int _reg_lighting_and_sound_cue_sheet = testing::register_test(
    "lighting_and_sound_cue_sheet",
    "1942_lighting_and_sound_cue_sheet: stub status check",
    &run_lighting_and_sound_cue_sheet);

}
