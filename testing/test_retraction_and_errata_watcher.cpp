// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/084_retraction_and_errata_watcher/retraction_and_errata_watcher.hpp"

namespace {

testing::TestOutcome run_retraction_and_errata_watcher() {
    retraction_and_errata_watcher::init();
    auto s = retraction_and_errata_watcher::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    retraction_and_errata_watcher::shutdown();
    return testing::ok();
}

const int _reg_retraction_and_errata_watcher = testing::register_test(
    "retraction_and_errata_watcher",
    "084_retraction_and_errata_watcher: stub status check",
    &run_retraction_and_errata_watcher);

}
