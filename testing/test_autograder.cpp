// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/518_autograder/autograder.hpp"

namespace {

testing::TestOutcome run_autograder() {
    autograder::init();
    auto s = autograder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    autograder::shutdown();
    return testing::ok();
}

const int _reg_autograder = testing::register_test(
    "autograder",
    "518_autograder: stub status check",
    &run_autograder);

}
