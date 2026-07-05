#include "test_runner.hpp"
#include "../modules/1750_room_acoustics_analyzer/room_acoustics_analyzer.hpp"

namespace {

testing::TestOutcome run_room_acoustics_analyzer() {
    room_acoustics_analyzer::init();
    auto s = room_acoustics_analyzer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    room_acoustics_analyzer::shutdown();
    return testing::ok();
}

const int _reg_room_acoustics_analyzer = testing::register_test(
    "room_acoustics_analyzer",
    "1750_room_acoustics_analyzer: stub status check",
    &run_room_acoustics_analyzer);

}
