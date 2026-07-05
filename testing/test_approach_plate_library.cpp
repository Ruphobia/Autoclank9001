// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1171_approach_plate_library/approach_plate_library.hpp"

namespace {

testing::TestOutcome run_approach_plate_library() {
    approach_plate_library::init();
    auto s = approach_plate_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    approach_plate_library::shutdown();
    return testing::ok();
}

const int _reg_approach_plate_library = testing::register_test(
    "approach_plate_library",
    "1171_approach_plate_library: stub status check",
    &run_approach_plate_library);

}
