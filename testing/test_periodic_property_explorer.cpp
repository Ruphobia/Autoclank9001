// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/984_periodic_property_explorer/periodic_property_explorer.hpp"

namespace {

testing::TestOutcome run_periodic_property_explorer() {
    periodic_property_explorer::init();
    auto s = periodic_property_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    periodic_property_explorer::shutdown();
    return testing::ok();
}

const int _reg_periodic_property_explorer = testing::register_test(
    "periodic_property_explorer",
    "984_periodic_property_explorer: stub status check",
    &run_periodic_property_explorer);

}
