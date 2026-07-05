// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1044_refrigeration_cycle_modeler/refrigeration_cycle_modeler.hpp"

namespace {

testing::TestOutcome run_refrigeration_cycle_modeler() {
    refrigeration_cycle_modeler::init();
    auto s = refrigeration_cycle_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    refrigeration_cycle_modeler::shutdown();
    return testing::ok();
}

const int _reg_refrigeration_cycle_modeler = testing::register_test(
    "refrigeration_cycle_modeler",
    "1044_refrigeration_cycle_modeler: stub status check",
    &run_refrigeration_cycle_modeler);

}
