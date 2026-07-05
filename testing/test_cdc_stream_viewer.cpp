// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/250_cdc_stream_viewer/cdc_stream_viewer.hpp"

namespace {

testing::TestOutcome run_cdc_stream_viewer() {
    cdc_stream_viewer::init();
    auto s = cdc_stream_viewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cdc_stream_viewer::shutdown();
    return testing::ok();
}

const int _reg_cdc_stream_viewer = testing::register_test(
    "cdc_stream_viewer",
    "250_cdc_stream_viewer: stub status check",
    &run_cdc_stream_viewer);

}
