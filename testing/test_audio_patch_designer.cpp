// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1917_audio_patch_designer/audio_patch_designer.hpp"

namespace {

testing::TestOutcome run_audio_patch_designer() {
    audio_patch_designer::init();
    auto s = audio_patch_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    audio_patch_designer::shutdown();
    return testing::ok();
}

const int _reg_audio_patch_designer = testing::register_test(
    "audio_patch_designer",
    "1917_audio_patch_designer: stub status check",
    &run_audio_patch_designer);

}
