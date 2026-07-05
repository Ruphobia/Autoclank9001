// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1895_reference_library_and_ocr_shelf/reference_library_and_ocr_shelf.hpp"

namespace {

testing::TestOutcome run_reference_library_and_ocr_shelf() {
    reference_library_and_ocr_shelf::init();
    auto s = reference_library_and_ocr_shelf::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    reference_library_and_ocr_shelf::shutdown();
    return testing::ok();
}

const int _reg_reference_library_and_ocr_shelf = testing::register_test(
    "reference_library_and_ocr_shelf",
    "1895_reference_library_and_ocr_shelf: stub status check",
    &run_reference_library_and_ocr_shelf);

}
