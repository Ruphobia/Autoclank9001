// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/213_dsp_and_filter_designer/dsp_and_filter_designer.hpp"

namespace {

testing::TestOutcome run_dsp_and_filter_designer() {
    dsp_and_filter_designer::init();
    auto s = dsp_and_filter_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dsp_and_filter_designer::shutdown();
    return testing::ok();
}

const int _reg_dsp_and_filter_designer = testing::register_test(
    "dsp_and_filter_designer",
    "213_dsp_and_filter_designer: stub status check",
    &run_dsp_and_filter_designer);

}
