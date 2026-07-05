// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/643_literature_review_tracker/literature_review_tracker.hpp"

namespace {

testing::TestOutcome run_literature_review_tracker() {
    literature_review_tracker::init();
    auto s = literature_review_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    literature_review_tracker::shutdown();
    return testing::ok();
}

const int _reg_literature_review_tracker = testing::register_test(
    "literature_review_tracker",
    "643_literature_review_tracker: stub status check",
    &run_literature_review_tracker);

}
