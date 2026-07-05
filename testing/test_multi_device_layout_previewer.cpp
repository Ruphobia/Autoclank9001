// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/166_multi_device_layout_previewer/multi_device_layout_previewer.hpp"

namespace {

testing::TestOutcome run_multi_device_layout_previewer() {
    multi_device_layout_previewer::init();
    auto s = multi_device_layout_previewer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    multi_device_layout_previewer::shutdown();
    return testing::ok();
}

const int _reg_multi_device_layout_previewer = testing::register_test(
    "multi_device_layout_previewer",
    "166_multi_device_layout_previewer: stub status check",
    &run_multi_device_layout_previewer);

}
