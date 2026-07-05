// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/735_polyglot_scripture_viewer/polyglot_scripture_viewer.hpp"

namespace {

testing::TestOutcome run_polyglot_scripture_viewer() {
    polyglot_scripture_viewer::init();
    auto s = polyglot_scripture_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polyglot_scripture_viewer::shutdown();
    return testing::ok();
}

const int _reg_polyglot_scripture_viewer = testing::register_test(
    "polyglot_scripture_viewer",
    "735_polyglot_scripture_viewer: stub status check",
    &run_polyglot_scripture_viewer);

}
