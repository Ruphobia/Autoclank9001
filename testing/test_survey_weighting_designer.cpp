// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1443_survey_weighting_designer/survey_weighting_designer.hpp"

namespace {

testing::TestOutcome run_survey_weighting_designer() {
    survey_weighting_designer::init();
    auto s = survey_weighting_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    survey_weighting_designer::shutdown();
    return testing::ok();
}

const int _reg_survey_weighting_designer = testing::register_test(
    "survey_weighting_designer",
    "1443_survey_weighting_designer: stub status check",
    &run_survey_weighting_designer);

}
