#include "test_runner.hpp"
#include "../modules/1950_world_bible/world_bible.hpp"

namespace {

testing::TestOutcome run_world_bible() {
    world_bible::init();
    auto s = world_bible::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    world_bible::shutdown();
    return testing::ok();
}

const int _reg_world_bible = testing::register_test(
    "world_bible",
    "1950_world_bible: stub status check",
    &run_world_bible);

}
