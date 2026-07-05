// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/600_id_ocr_pipeline/id_ocr_pipeline.hpp"

namespace {

testing::TestOutcome run_id_ocr_pipeline() {
    id_ocr_pipeline::init();
    auto s = id_ocr_pipeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    id_ocr_pipeline::shutdown();
    return testing::ok();
}

const int _reg_id_ocr_pipeline = testing::register_test(
    "id_ocr_pipeline",
    "600_id_ocr_pipeline: stub status check",
    &run_id_ocr_pipeline);

}
