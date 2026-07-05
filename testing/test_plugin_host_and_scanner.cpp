// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1752_plugin_host_and_scanner/plugin_host_and_scanner.hpp"

namespace {

testing::TestOutcome run_plugin_host_and_scanner() {
    plugin_host_and_scanner::init();
    auto s = plugin_host_and_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plugin_host_and_scanner::shutdown();
    return testing::ok();
}

const int _reg_plugin_host_and_scanner = testing::register_test(
    "plugin_host_and_scanner",
    "1752_plugin_host_and_scanner: stub status check",
    &run_plugin_host_and_scanner);

}
