// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1732_parametric_and_graphic_eq/parametric_and_graphic_eq.hpp"

namespace {

testing::TestOutcome run_parametric_and_graphic_eq() {
    parametric_and_graphic_eq::init();
    auto s = parametric_and_graphic_eq::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    parametric_and_graphic_eq::shutdown();
    return testing::ok();
}

const int _reg_parametric_and_graphic_eq = testing::register_test(
    "parametric_and_graphic_eq",
    "1732_parametric_and_graphic_eq: stub status check",
    &run_parametric_and_graphic_eq);

}
