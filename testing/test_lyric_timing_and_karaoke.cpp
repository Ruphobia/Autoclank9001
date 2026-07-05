#include "test_runner.hpp"
#include "../modules/838_lyric_timing_and_karaoke/lyric_timing_and_karaoke.hpp"

namespace {

testing::TestOutcome run_lyric_timing_and_karaoke() {
    lyric_timing_and_karaoke::init();
    auto s = lyric_timing_and_karaoke::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lyric_timing_and_karaoke::shutdown();
    return testing::ok();
}

const int _reg_lyric_timing_and_karaoke = testing::register_test(
    "lyric_timing_and_karaoke",
    "838_lyric_timing_and_karaoke: stub status check",
    &run_lyric_timing_and_karaoke);

}
