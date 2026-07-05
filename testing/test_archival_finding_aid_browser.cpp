// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/690_archival_finding_aid_browser/archival_finding_aid_browser.hpp"

namespace {

testing::TestOutcome run_archival_finding_aid_browser() {
    archival_finding_aid_browser::init();
    auto s = archival_finding_aid_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    archival_finding_aid_browser::shutdown();
    return testing::ok();
}

const int _reg_archival_finding_aid_browser = testing::register_test(
    "archival_finding_aid_browser",
    "690_archival_finding_aid_browser: stub status check",
    &run_archival_finding_aid_browser);

}
