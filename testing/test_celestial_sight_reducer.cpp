// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2102_celestial_sight_reducer/celestial_sight_reducer.hpp"

namespace {

testing::TestOutcome run_celestial_sight_reducer() {
    celestial_sight_reducer::init();
    auto s = celestial_sight_reducer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    celestial_sight_reducer::shutdown();
    return testing::ok();
}

const int _reg_celestial_sight_reducer = testing::register_test(
    "celestial_sight_reducer",
    "2102_celestial_sight_reducer: stub status check",
    &run_celestial_sight_reducer);

}
