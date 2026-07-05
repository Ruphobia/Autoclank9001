// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/954_ocr_for_drawings/ocr_for_drawings.hpp"

namespace {

testing::TestOutcome run_ocr_for_drawings() {
    ocr_for_drawings::init();
    auto s = ocr_for_drawings::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr_for_drawings::shutdown();
    return testing::ok();
}

const int _reg_ocr_for_drawings = testing::register_test(
    "ocr_for_drawings",
    "954_ocr_for_drawings: stub status check",
    &run_ocr_for_drawings);

}
