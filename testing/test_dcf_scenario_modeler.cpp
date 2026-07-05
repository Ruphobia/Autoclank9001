// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/560_dcf_scenario_modeler/dcf_scenario_modeler.hpp"

namespace {

testing::TestOutcome run_dcf_scenario_modeler() {
    dcf_scenario_modeler::init();
    auto s = dcf_scenario_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dcf_scenario_modeler::shutdown();
    return testing::ok();
}

const int _reg_dcf_scenario_modeler = testing::register_test(
    "dcf_scenario_modeler",
    "560_dcf_scenario_modeler: stub status check",
    &run_dcf_scenario_modeler);

}
