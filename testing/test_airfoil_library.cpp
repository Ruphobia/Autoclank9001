// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1147_airfoil_library/airfoil_library.hpp"

namespace {

testing::TestOutcome run_airfoil_library() {
    airfoil_library::init();
    auto s = airfoil_library::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    airfoil_library::shutdown();
    return testing::ok();
}

const int _reg_airfoil_library = testing::register_test(
    "airfoil_library",
    "1147_airfoil_library: stub status check",
    &run_airfoil_library);

}
