#include "test_runner.hpp"
#include "../modules/2083_play_by_play_loader/play_by_play_loader.hpp"

namespace {

testing::TestOutcome run_play_by_play_loader() {
    play_by_play_loader::init();
    auto s = play_by_play_loader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    play_by_play_loader::shutdown();
    return testing::ok();
}

const int _reg_play_by_play_loader = testing::register_test(
    "play_by_play_loader",
    "2083_play_by_play_loader: stub status check",
    &run_play_by_play_loader);

}
