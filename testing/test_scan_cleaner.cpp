// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/624_scan_cleaner/scan_cleaner.hpp"

namespace {

testing::TestOutcome run_scan_cleaner() {
    scan_cleaner::init();
    auto s = scan_cleaner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    scan_cleaner::shutdown();
    return testing::ok();
}

const int _reg_scan_cleaner = testing::register_test(
    "scan_cleaner",
    "624_scan_cleaner: stub status check",
    &run_scan_cleaner);

}
