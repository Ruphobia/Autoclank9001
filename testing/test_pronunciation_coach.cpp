// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/825_pronunciation_coach/pronunciation_coach.hpp"

namespace {

testing::TestOutcome run_pronunciation_coach() {
    pronunciation_coach::init();
    auto s = pronunciation_coach::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pronunciation_coach::shutdown();
    return testing::ok();
}

const int _reg_pronunciation_coach = testing::register_test(
    "pronunciation_coach",
    "825_pronunciation_coach: stub status check",
    &run_pronunciation_coach);

}
