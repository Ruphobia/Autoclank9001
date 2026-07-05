// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/528_ocr_grader/ocr_grader.hpp"

namespace {

testing::TestOutcome run_ocr_grader() {
    ocr_grader::init();
    auto s = ocr_grader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr_grader::shutdown();
    return testing::ok();
}

const int _reg_ocr_grader = testing::register_test(
    "ocr_grader",
    "528_ocr_grader: stub status check",
    &run_ocr_grader);

}
