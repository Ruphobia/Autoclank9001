// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/203_errata_tracker/errata_tracker.hpp"

namespace {

testing::TestOutcome run_errata_tracker() {
    errata_tracker::init();
    auto s = errata_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    errata_tracker::shutdown();
    return testing::ok();
}

const int _reg_errata_tracker = testing::register_test(
    "errata_tracker",
    "203_errata_tracker: stub status check",
    &run_errata_tracker);

}
