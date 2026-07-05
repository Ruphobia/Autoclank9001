// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/652_retraction_and_errata_watch/retraction_and_errata_watch.hpp"

namespace {

testing::TestOutcome run_retraction_and_errata_watch() {
    retraction_and_errata_watch::init();
    auto s = retraction_and_errata_watch::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    retraction_and_errata_watch::shutdown();
    return testing::ok();
}

const int _reg_retraction_and_errata_watch = testing::register_test(
    "retraction_and_errata_watch",
    "652_retraction_and_errata_watch: stub status check",
    &run_retraction_and_errata_watch);

}
