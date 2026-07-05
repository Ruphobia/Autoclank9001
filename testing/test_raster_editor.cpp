// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1622_raster_editor/raster_editor.hpp"

namespace {

testing::TestOutcome run_raster_editor() {
    raster_editor::init();
    auto s = raster_editor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    raster_editor::shutdown();
    return testing::ok();
}

const int _reg_raster_editor = testing::register_test(
    "raster_editor",
    "1622_raster_editor: stub status check",
    &run_raster_editor);

}
