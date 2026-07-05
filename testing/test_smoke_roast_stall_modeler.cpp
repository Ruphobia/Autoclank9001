// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1799_smoke_roast_stall_modeler/smoke_roast_stall_modeler.hpp"

namespace {

testing::TestOutcome run_smoke_roast_stall_modeler() {
    smoke_roast_stall_modeler::init();
    auto s = smoke_roast_stall_modeler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    smoke_roast_stall_modeler::shutdown();
    return testing::ok();
}

const int _reg_smoke_roast_stall_modeler = testing::register_test(
    "smoke_roast_stall_modeler",
    "1799_smoke_roast_stall_modeler: stub status check",
    &run_smoke_roast_stall_modeler);

}
