// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1010_room_acoustics_modeler/room_acoustics_modeler.hpp"

namespace {

testing::TestOutcome run_room_acoustics_modeler() {
    room_acoustics_modeler::init();
    auto s = room_acoustics_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    room_acoustics_modeler::shutdown();
    return testing::ok();
}

const int _reg_room_acoustics_modeler = testing::register_test(
    "room_acoustics_modeler",
    "1010_room_acoustics_modeler: stub status check",
    &run_room_acoustics_modeler);

}
