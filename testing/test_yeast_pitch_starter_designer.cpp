// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1790_yeast_pitch_starter_designer/yeast_pitch_starter_designer.hpp"

namespace {

testing::TestOutcome run_yeast_pitch_starter_designer() {
    yeast_pitch_starter_designer::init();
    auto s = yeast_pitch_starter_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    yeast_pitch_starter_designer::shutdown();
    return testing::ok();
}

const int _reg_yeast_pitch_starter_designer = testing::register_test(
    "yeast_pitch_starter_designer",
    "1790_yeast_pitch_starter_designer: stub status check",
    &run_yeast_pitch_starter_designer);

}
