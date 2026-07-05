// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/312_spatial_audio_mixer/spatial_audio_mixer.hpp"

namespace {

testing::TestOutcome run_spatial_audio_mixer() {
    spatial_audio_mixer::init();
    auto s = spatial_audio_mixer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    spatial_audio_mixer::shutdown();
    return testing::ok();
}

const int _reg_spatial_audio_mixer = testing::register_test(
    "spatial_audio_mixer",
    "312_spatial_audio_mixer: stub status check",
    &run_spatial_audio_mixer);

}
