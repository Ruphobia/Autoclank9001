// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2001_practice_logger/practice_logger.hpp"

namespace {

testing::TestOutcome run_practice_logger() {
    practice_logger::init();
    auto s = practice_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    practice_logger::shutdown();
    return testing::ok();
}

const int _reg_practice_logger = testing::register_test(
    "practice_logger",
    "2001_practice_logger: stub status check",
    &run_practice_logger);

}
