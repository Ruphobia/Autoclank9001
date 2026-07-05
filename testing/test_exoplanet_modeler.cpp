// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1492_exoplanet_modeler/exoplanet_modeler.hpp"

namespace {

testing::TestOutcome run_exoplanet_modeler() {
    exoplanet_modeler::init();
    auto s = exoplanet_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    exoplanet_modeler::shutdown();
    return testing::ok();
}

const int _reg_exoplanet_modeler = testing::register_test(
    "exoplanet_modeler",
    "1492_exoplanet_modeler: stub status check",
    &run_exoplanet_modeler);

}
