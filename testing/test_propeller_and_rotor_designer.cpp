// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1162_propeller_and_rotor_designer/propeller_and_rotor_designer.hpp"

namespace {

testing::TestOutcome run_propeller_and_rotor_designer() {
    propeller_and_rotor_designer::init();
    auto s = propeller_and_rotor_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    propeller_and_rotor_designer::shutdown();
    return testing::ok();
}

const int _reg_propeller_and_rotor_designer = testing::register_test(
    "propeller_and_rotor_designer",
    "1162_propeller_and_rotor_designer: stub status check",
    &run_propeller_and_rotor_designer);

}
