#include "test_runner.hpp"
#include "../modules/2095_tidal_stream_atlas/tidal_stream_atlas.hpp"

namespace {

testing::TestOutcome run_tidal_stream_atlas() {
    tidal_stream_atlas::init();
    auto s = tidal_stream_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tidal_stream_atlas::shutdown();
    return testing::ok();
}

const int _reg_tidal_stream_atlas = testing::register_test(
    "tidal_stream_atlas",
    "2095_tidal_stream_atlas: stub status check",
    &run_tidal_stream_atlas);

}
