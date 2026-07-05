// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2135_oral_history_studio/oral_history_studio.hpp"

namespace {

testing::TestOutcome run_oral_history_studio() {
    oral_history_studio::init();
    auto s = oral_history_studio::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    oral_history_studio::shutdown();
    return testing::ok();
}

const int _reg_oral_history_studio = testing::register_test(
    "oral_history_studio",
    "2135_oral_history_studio: stub status check",
    &run_oral_history_studio);

}
