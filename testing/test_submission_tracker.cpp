// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/512_submission_tracker/submission_tracker.hpp"

namespace {

testing::TestOutcome run_submission_tracker() {
    submission_tracker::init();
    auto s = submission_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    submission_tracker::shutdown();
    return testing::ok();
}

const int _reg_submission_tracker = testing::register_test(
    "submission_tracker",
    "512_submission_tracker: stub status check",
    &run_submission_tracker);

}
