#include "test_runner.hpp"
#include "../modules/1982_replay_scrubber/replay_scrubber.hpp"

namespace {

testing::TestOutcome run_replay_scrubber() {
    replay_scrubber::init();
    auto s = replay_scrubber::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    replay_scrubber::shutdown();
    return testing::ok();
}

const int _reg_replay_scrubber = testing::register_test(
    "replay_scrubber",
    "1982_replay_scrubber: stub status check",
    &run_replay_scrubber);

}
