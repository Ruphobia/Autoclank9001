// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2044_hunt_and_catch_log/hunt_and_catch_log.hpp"

namespace {

testing::TestOutcome run_hunt_and_catch_log() {
    hunt_and_catch_log::init();
    auto s = hunt_and_catch_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hunt_and_catch_log::shutdown();
    return testing::ok();
}

const int _reg_hunt_and_catch_log = testing::register_test(
    "hunt_and_catch_log",
    "2044_hunt_and_catch_log: stub status check",
    &run_hunt_and_catch_log);

}
