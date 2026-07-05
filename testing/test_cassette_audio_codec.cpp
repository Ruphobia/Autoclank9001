#include "test_runner.hpp"
#include "../modules/404_cassette_audio_codec/cassette_audio_codec.hpp"

namespace {

testing::TestOutcome run_cassette_audio_codec() {
    cassette_audio_codec::init();
    auto s = cassette_audio_codec::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cassette_audio_codec::shutdown();
    return testing::ok();
}

const int _reg_cassette_audio_codec = testing::register_test(
    "cassette_audio_codec",
    "404_cassette_audio_codec: stub status check",
    &run_cassette_audio_codec);

}
