// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/548_reinsurance_treaty_modeler/reinsurance_treaty_modeler.hpp"

namespace {

testing::TestOutcome run_reinsurance_treaty_modeler() {
    reinsurance_treaty_modeler::init();
    auto s = reinsurance_treaty_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reinsurance_treaty_modeler::shutdown();
    return testing::ok();
}

const int _reg_reinsurance_treaty_modeler = testing::register_test(
    "reinsurance_treaty_modeler",
    "548_reinsurance_treaty_modeler: stub status check",
    &run_reinsurance_treaty_modeler);

}
