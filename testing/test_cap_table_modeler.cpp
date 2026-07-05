// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/567_cap_table_modeler/cap_table_modeler.hpp"

namespace {

testing::TestOutcome run_cap_table_modeler() {
    cap_table_modeler::init();
    auto s = cap_table_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cap_table_modeler::shutdown();
    return testing::ok();
}

const int _reg_cap_table_modeler = testing::register_test(
    "cap_table_modeler",
    "567_cap_table_modeler: stub status check",
    &run_cap_table_modeler);

}
