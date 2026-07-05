// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/327_webxr_live_preview/webxr_live_preview.hpp"

namespace {

testing::TestOutcome run_webxr_live_preview() {
    webxr_live_preview::init();
    auto s = webxr_live_preview::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    webxr_live_preview::shutdown();
    return testing::ok();
}

const int _reg_webxr_live_preview = testing::register_test(
    "webxr_live_preview",
    "327_webxr_live_preview: stub status check",
    &run_webxr_live_preview);

}
