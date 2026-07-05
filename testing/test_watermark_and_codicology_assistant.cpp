// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/701_watermark_and_codicology_assistant/watermark_and_codicology_assistant.hpp"

namespace {

testing::TestOutcome run_watermark_and_codicology_assistant() {
    watermark_and_codicology_assistant::init();
    auto s = watermark_and_codicology_assistant::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    watermark_and_codicology_assistant::shutdown();
    return testing::ok();
}

const int _reg_watermark_and_codicology_assistant = testing::register_test(
    "watermark_and_codicology_assistant",
    "701_watermark_and_codicology_assistant: stub status check",
    &run_watermark_and_codicology_assistant);

}
