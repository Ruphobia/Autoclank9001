// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1897_skin_tone_preview/skin_tone_preview.hpp"

namespace {

testing::TestOutcome run_skin_tone_preview() {
    skin_tone_preview::init();
    auto s = skin_tone_preview::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    skin_tone_preview::shutdown();
    return testing::ok();
}

const int _reg_skin_tone_preview = testing::register_test(
    "skin_tone_preview",
    "1897_skin_tone_preview: stub status check",
    &run_skin_tone_preview);

}
