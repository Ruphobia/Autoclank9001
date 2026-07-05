// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1337_molecular_docking/molecular_docking.hpp"

namespace {

testing::TestOutcome run_molecular_docking() {
    molecular_docking::init();
    auto s = molecular_docking::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    molecular_docking::shutdown();
    return testing::ok();
}

const int _reg_molecular_docking = testing::register_test(
    "molecular_docking",
    "1337_molecular_docking: stub status check",
    &run_molecular_docking);

}
