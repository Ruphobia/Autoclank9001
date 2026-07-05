// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1204_engine_geometry_lab/engine_geometry_lab.hpp"

namespace {

testing::TestOutcome run_engine_geometry_lab() {
    engine_geometry_lab::init();
    auto s = engine_geometry_lab::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    engine_geometry_lab::shutdown();
    return testing::ok();
}

const int _reg_engine_geometry_lab = testing::register_test(
    "engine_geometry_lab",
    "1204_engine_geometry_lab: stub status check",
    &run_engine_geometry_lab);

}
