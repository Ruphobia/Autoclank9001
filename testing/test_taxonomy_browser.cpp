// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1529_taxonomy_browser/taxonomy_browser.hpp"

namespace {

testing::TestOutcome run_taxonomy_browser() {
    taxonomy_browser::init();
    auto s = taxonomy_browser::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    taxonomy_browser::shutdown();
    return testing::ok();
}

const int _reg_taxonomy_browser = testing::register_test(
    "taxonomy_browser",
    "1529_taxonomy_browser: stub status check",
    &run_taxonomy_browser);

}
