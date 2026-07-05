// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/619_ocr_engine/ocr_engine.hpp"

namespace {

testing::TestOutcome run_ocr_engine() {
    ocr_engine::init();
    auto s = ocr_engine::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ocr_engine::shutdown();
    return testing::ok();
}

const int _reg_ocr_engine = testing::register_test(
    "ocr_engine",
    "619_ocr_engine: stub status check",
    &run_ocr_engine);

}
