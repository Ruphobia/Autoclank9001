// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/126_route_map/route_map.hpp"

namespace {

testing::TestOutcome run_route_map() {
    route_map::init();
    auto s = route_map::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    route_map::shutdown();
    return testing::ok();
}

const int _reg_route_map = testing::register_test(
    "route_map",
    "126_route_map: stub status check",
    &run_route_map);

}
