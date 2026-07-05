// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/117_live_dev_server/live_dev_server.hpp"

namespace {

testing::TestOutcome run_live_dev_server() {
    live_dev_server::init();
    auto s = live_dev_server::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    live_dev_server::shutdown();
    return testing::ok();
}

const int _reg_live_dev_server = testing::register_test(
    "live_dev_server",
    "117_live_dev_server: stub status check",
    &run_live_dev_server);

}
