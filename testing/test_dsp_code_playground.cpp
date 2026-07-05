// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1757_dsp_code_playground/dsp_code_playground.hpp"

namespace {

testing::TestOutcome run_dsp_code_playground() {
    dsp_code_playground::init();
    auto s = dsp_code_playground::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dsp_code_playground::shutdown();
    return testing::ok();
}

const int _reg_dsp_code_playground = testing::register_test(
    "dsp_code_playground",
    "1757_dsp_code_playground: stub status check",
    &run_dsp_code_playground);

}
