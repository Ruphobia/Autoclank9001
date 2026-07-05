// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/134_websocket_explorer/websocket_explorer.hpp"

namespace {

testing::TestOutcome run_websocket_explorer() {
    websocket_explorer::init();
    auto s = websocket_explorer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    websocket_explorer::shutdown();
    return testing::ok();
}

const int _reg_websocket_explorer = testing::register_test(
    "websocket_explorer",
    "134_websocket_explorer: stub status check",
    &run_websocket_explorer);

}
