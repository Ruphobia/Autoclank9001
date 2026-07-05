// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1937_audience_beat_sheet/audience_beat_sheet.hpp"

namespace {

testing::TestOutcome run_audience_beat_sheet() {
    audience_beat_sheet::init();
    auto s = audience_beat_sheet::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    audience_beat_sheet::shutdown();
    return testing::ok();
}

const int _reg_audience_beat_sheet = testing::register_test(
    "audience_beat_sheet",
    "1937_audience_beat_sheet: stub status check",
    &run_audience_beat_sheet);

}
