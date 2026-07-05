// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2201_music_history_and_recognition/music_history_and_recognition.hpp"

namespace {

testing::TestOutcome run_music_history_and_recognition() {
    music_history_and_recognition::init();
    auto s = music_history_and_recognition::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    music_history_and_recognition::shutdown();
    return testing::ok();
}

const int _reg_music_history_and_recognition = testing::register_test(
    "music_history_and_recognition",
    "2201_music_history_and_recognition: stub status check",
    &run_music_history_and_recognition);

}
