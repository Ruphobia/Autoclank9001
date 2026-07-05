// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/927_step_iges_converter/step_iges_converter.hpp"

namespace {

testing::TestOutcome run_step_iges_converter() {
    step_iges_converter::init();
    auto s = step_iges_converter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    step_iges_converter::shutdown();
    return testing::ok();
}

const int _reg_step_iges_converter = testing::register_test(
    "step_iges_converter",
    "927_step_iges_converter: stub status check",
    &run_step_iges_converter);

}
