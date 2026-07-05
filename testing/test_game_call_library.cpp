#include "test_runner.hpp"
#include "../modules/2054_game_call_library/game_call_library.hpp"

namespace {

testing::TestOutcome run_game_call_library() {
    game_call_library::init();
    auto s = game_call_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    game_call_library::shutdown();
    return testing::ok();
}

const int _reg_game_call_library = testing::register_test(
    "game_call_library",
    "2054_game_call_library: stub status check",
    &run_game_call_library);

}
