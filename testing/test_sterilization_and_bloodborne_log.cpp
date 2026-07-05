// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1907_sterilization_and_bloodborne_log/sterilization_and_bloodborne_log.hpp"

namespace {

testing::TestOutcome run_sterilization_and_bloodborne_log() {
    sterilization_and_bloodborne_log::init();
    auto s = sterilization_and_bloodborne_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sterilization_and_bloodborne_log::shutdown();
    return testing::ok();
}

const int _reg_sterilization_and_bloodborne_log = testing::register_test(
    "sterilization_and_bloodborne_log",
    "1907_sterilization_and_bloodborne_log: stub status check",
    &run_sterilization_and_bloodborne_log);

}
