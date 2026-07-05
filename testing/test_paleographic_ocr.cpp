// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/707_paleographic_ocr/paleographic_ocr.hpp"

namespace {

testing::TestOutcome run_paleographic_ocr() {
    paleographic_ocr::init();
    auto s = paleographic_ocr::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    paleographic_ocr::shutdown();
    return testing::ok();
}

const int _reg_paleographic_ocr = testing::register_test(
    "paleographic_ocr",
    "707_paleographic_ocr: stub status check",
    &run_paleographic_ocr);

}
