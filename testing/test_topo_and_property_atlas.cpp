// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2049_topo_and_property_atlas/topo_and_property_atlas.hpp"

namespace {

testing::TestOutcome run_topo_and_property_atlas() {
    topo_and_property_atlas::init();
    auto s = topo_and_property_atlas::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    topo_and_property_atlas::shutdown();
    return testing::ok();
}

const int _reg_topo_and_property_atlas = testing::register_test(
    "topo_and_property_atlas",
    "2049_topo_and_property_atlas: stub status check",
    &run_topo_and_property_atlas);

}
