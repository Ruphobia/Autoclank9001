// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/953_plm_revision_tracker/plm_revision_tracker.hpp"

namespace {

testing::TestOutcome run_plm_revision_tracker() {
    plm_revision_tracker::init();
    auto s = plm_revision_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    plm_revision_tracker::shutdown();
    return testing::ok();
}

const int _reg_plm_revision_tracker = testing::register_test(
    "plm_revision_tracker",
    "953_plm_revision_tracker: stub status check",
    &run_plm_revision_tracker);

}
