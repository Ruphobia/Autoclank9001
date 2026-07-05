// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1827_winetastingnotes/winetastingnotes.hpp"

namespace {

testing::TestOutcome run_winetastingnotes() {
    winetastingnotes::init();
    auto s = winetastingnotes::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    winetastingnotes::shutdown();
    return testing::ok();
}

const int _reg_winetastingnotes = testing::register_test(
    "winetastingnotes",
    "1827_winetastingnotes: stub status check",
    &run_winetastingnotes);

}
