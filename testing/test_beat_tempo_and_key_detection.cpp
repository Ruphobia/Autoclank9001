#include "test_runner.hpp"
#include "../modules/1743_beat_tempo_and_key_detection/beat_tempo_and_key_detection.hpp"

namespace {

testing::TestOutcome run_beat_tempo_and_key_detection() {
    beat_tempo_and_key_detection::init();
    auto s = beat_tempo_and_key_detection::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    beat_tempo_and_key_detection::shutdown();
    return testing::ok();
}

const int _reg_beat_tempo_and_key_detection = testing::register_test(
    "beat_tempo_and_key_detection",
    "1743_beat_tempo_and_key_detection: stub status check",
    &run_beat_tempo_and_key_detection);

}
